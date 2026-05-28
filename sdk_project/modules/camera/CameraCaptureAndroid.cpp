#include "CameraCapture.h"
#include <iostream>
#include "rhi/gles/EGLCore.h"
#include "rhi/gles/EGLWindow.h"
#include "hal/codec/AndroidMediaCodecEncoder.h"

#ifdef ANDROID
#include <camera/NdkCameraManager.h>
#include <media/NdkImageReader.h>
#else
#include "camera/NdkCameraManager.h"
#include "media/NdkImageReader.h"
#endif

namespace video_sdk {
namespace modules {

/**
 * @brief Android 工业级采集实现，直接调用 NDK Camera2 API。
 * 避开 Java 层的 SurfaceTexture 回调带来的跨语言 JNI 损耗，
 * 采用 AImageReader 直接在 Native 层接收硬件吐出的 Buffer。
 * 增加了预览渲染 (Preview) 和 录像 (Record) 的基础架构支持。
 */
class CameraCaptureAndroid : public ICameraCaptureImpl {
public:
    CameraCaptureAndroid() {}
    ~CameraCaptureAndroid() override { stopPreview(); }

    bool initialize(int width, int height, int fps) override {
        m_width = width;
        m_height = height;
        m_fps = fps;
        m_cameraManager = ACameraManager_create();
        if (!m_cameraManager) {
            std::cerr << "[NDK Camera] Failed to create ACameraManager" << std::endl;
            return false;
        }

        // 1. 设置 ImageReader 接收回调
        AImageReader_new(m_width, m_height, 35 /*YUV_420_888*/, 2, &m_imageReader);

        AImageReader_ImageListener listener;
        listener.context = this;
        listener.onImageAvailable = onImageAvailableCallback;
        AImageReader_setImageListener(m_imageReader, &listener);

        AImageReader_getWindow(m_imageReader, &m_nativeWindow);

        // 2. 初始化 EGL 环境
        m_eglCore = std::make_unique<rhi::EGLCore>();

        return true;
    }

    void startPreview() override {
        if (m_isPreviewing) return;

        ACameraDevice_StateCallbacks deviceCallbacks;
        deviceCallbacks.context = this;
        deviceCallbacks.onDisconnected = [](void*, ACameraDevice*){};
        deviceCallbacks.onError = [](void*, ACameraDevice*, int){};

        if (ACameraManager_openCamera(m_cameraManager, "0", &deviceCallbacks, &m_cameraDevice) != ACAMERA_OK) {
            std::cerr << "[NDK Camera] Failed to open camera" << std::endl;
            return;
        }

        ACaptureSessionOutputContainer_create(&m_outputContainer);
        ACaptureSessionOutput_create(m_nativeWindow, &m_sessionOutput);
        ACaptureSessionOutputContainer_add(m_outputContainer, m_sessionOutput);

        ACameraOutputTarget_create(m_nativeWindow, &m_reqTarget);

        ACameraCaptureSession_stateCallbacks sessionCallbacks{};
        ACameraDevice_createCaptureSession(m_cameraDevice, m_outputContainer, &sessionCallbacks, &m_captureSession);

        ACameraDevice_createCaptureRequest(m_cameraDevice, TEMPLATE_PREVIEW, &m_captureRequest);
        ACaptureRequest_addTarget(m_captureRequest, m_reqTarget);

        ACameraCaptureSession_setRepeatingRequest(m_captureSession, nullptr, 1, &m_captureRequest, nullptr);

        m_isPreviewing = true;
        std::cout << "[NDK Camera] Camera2 preview started successfully!" << std::endl;
    }

    void stopPreview() override {
        if (!m_isPreviewing) return;

        stopRecording(); // 确保停止录制

        if (m_captureSession) { ACameraCaptureSession_close(m_captureSession); m_captureSession = nullptr; }
        if (m_captureRequest) { ACaptureRequest_free(m_captureRequest); m_captureRequest = nullptr; }
        if (m_cameraDevice) { ACameraDevice_close(m_cameraDevice); m_cameraDevice = nullptr; }
        if (m_outputContainer) { ACaptureSessionOutputContainer_free(m_outputContainer); m_outputContainer = nullptr; }
        if (m_sessionOutput) { ACaptureSessionOutput_free(m_sessionOutput); m_sessionOutput = nullptr; }
        if (m_reqTarget) { ACameraOutputTarget_free(m_reqTarget); m_reqTarget = nullptr; }
        if (m_imageReader) { AImageReader_delete(m_imageReader); m_imageReader = nullptr; }
        if (m_cameraManager) { ACameraManager_delete(m_cameraManager); m_cameraManager = nullptr; }

        m_isPreviewing = false;
        std::cout << "[NDK Camera] Camera2 preview stopped." << std::endl;
    }

    void switchCamera() override {
        // 先停掉当前的设备，切换 Camera ID 再启动
    }

    void setFrameCallback(OnFrameAvailableCallback callback) override {
        m_frameCallback = callback;
    }

    // 新增：设置预览 Surface (由外部传入，例如 Android 的 SurfaceView)
    void setPreviewSurface(void* nativeWindow) {
        if (m_eglCore && nativeWindow) {
            m_previewWindow = std::make_unique<rhi::EGLWindow>(m_eglCore.get(), nativeWindow);
        }
    }

    // 新增：开始录制
    void startRecording(const std::string& outputPath) {
        if (m_isRecording) return;
        m_encoder = std::make_unique<hal::AndroidMediaCodecEncoder>();
        m_encoder->initialize(outputPath, m_width, m_height, m_fps, 2000000); // 2Mbps
        m_encoder->start();
        m_isRecording = true;
        std::cout << "[NDK Camera] Started recording to " << outputPath << std::endl;
    }

    // 新增：停止录制
    void stopRecording() {
        if (!m_isRecording) return;
        if (m_encoder) {
            m_encoder->stop();
            m_encoder.reset();
        }
        m_isRecording = false;
        std::cout << "[NDK Camera] Stopped recording." << std::endl;
    }

private:
    static void onImageAvailableCallback(void* context, AImageReader* reader) {
        auto* self = static_cast<CameraCaptureAndroid*>(context);
        AImage* image = nullptr;
        if (AImageReader_acquireNextImage(reader, &image) == 0 && image) {

            // 1. 将 YUV 转为 OpenGL Texture (省略具体 OpenGL 调用，仅写流程)
            uint32_t currentTextureId = 0; // Fake texture ID for now
            int64_t pts = 0; // AImage_getTimestamp(image, &pts);

            // 2. 预览 (Preview): 渲染到屏幕
            if (self->m_previewWindow) {
                self->m_previewWindow->makeCurrent();
                // OpenGL 绘制 currentTextureId 到当前 Framebuffer
                self->m_previewWindow->swapBuffers();
            }

            // 3. 录像 (Record): 将纹理送入编码器
            if (self->m_isRecording && self->m_encoder) {
                self->m_encoder->encodeTexture(currentTextureId, pts);
            }

            // 4. 回调给业务层 (如果需要挂滤镜等)
            if (self->m_frameCallback) {
                self->m_frameCallback(currentTextureId, pts);
            }

            AImage_delete(image);
        }
    }

private:
    int m_width = 0, m_height = 0, m_fps = 30;
    bool m_isPreviewing = false;
    bool m_isRecording = false;
    OnFrameAvailableCallback m_frameCallback;

    ACameraManager* m_cameraManager = nullptr;
    ACameraDevice* m_cameraDevice = nullptr;
    AImageReader* m_imageReader = nullptr;
    ANativeWindow* m_nativeWindow = nullptr;

    ACaptureSessionOutputContainer* m_outputContainer = nullptr;
    ACaptureSessionOutput* m_sessionOutput = nullptr;
    ACameraOutputTarget* m_reqTarget = nullptr;
    ACaptureRequest* m_captureRequest = nullptr;
    ACameraCaptureSession* m_captureSession = nullptr;

    std::unique_ptr<rhi::EGLCore> m_eglCore;
    std::unique_ptr<rhi::EGLWindow> m_previewWindow;
    std::unique_ptr<hal::IMediaEncoder> m_encoder;
};

std::unique_ptr<ICameraCaptureImpl> CreatePlatformCameraImpl() {
    return std::make_unique<CameraCaptureAndroid>();
}

} // namespace modules
} // namespace video_sdk
