#include "CameraCapture.h"
#include <iostream>

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
 */
class CameraCaptureAndroid : public ICameraCaptureImpl {
public:
    CameraCaptureAndroid() {}
    ~CameraCaptureAndroid() override { stopPreview(); }

    bool initialize(int width, int height, int fps) override {
        m_width = width;
        m_height = height;
        m_cameraManager = ACameraManager_create();
        if (!m_cameraManager) {
            std::cerr << "[NDK Camera] Failed to create ACameraManager" << std::endl;
            return false;
        }

        // 1. 设置 ImageReader 接收回调
        // 实际开发中通常使用 AIMAGE_FORMAT_YUV_420_888
        AImageReader_new(m_width, m_height, 35 /*YUV_420_888*/, 2, &m_imageReader);

        AImageReader_ImageListener listener;
        listener.context = this;
        listener.onImageAvailable = onImageAvailableCallback;
        AImageReader_setImageListener(m_imageReader, &listener);

        AImageReader_getWindow(m_imageReader, &m_nativeWindow);
        return true;
    }

    void startPreview() override {
        if (m_isPreviewing) return;

        // 2. 挂载设备状态回调
        ACameraDevice_StateCallbacks deviceCallbacks;
        deviceCallbacks.context = this;
        deviceCallbacks.onDisconnected = [](void*, ACameraDevice*){};
        deviceCallbacks.onError = [](void*, ACameraDevice*, int){};

        // 简写：默认打开 ID 为 "0" 的后置摄像头
        if (ACameraManager_openCamera(m_cameraManager, "0", &deviceCallbacks, &m_cameraDevice) != ACAMERA_OK) {
            std::cerr << "[NDK Camera] Failed to open camera" << std::endl;
            return;
        }

        // 3. 配置输出目标 (Output Target)
        ACaptureSessionOutputContainer_create(&m_outputContainer);
        ACaptureSessionOutput_create(m_nativeWindow, &m_sessionOutput);
        ACaptureSessionOutputContainer_add(m_outputContainer, m_sessionOutput);

        ACameraOutputTarget_create(m_nativeWindow, &m_reqTarget);

        // 4. 创建 Session 并开始推流 (Repeating Request)
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

private:
    static void onImageAvailableCallback(void* context, AImageReader* reader) {
        auto* self = static_cast<CameraCaptureAndroid*>(context);
        AImage* image = nullptr;
        if (AImageReader_acquireNextImage(reader, &image) == 0 && image) {
            // 这里获得了原生的硬件帧。
            // 工业级实现：在这里取出 YUV data，再通过 GPU 离屏渲染将其包装为 FBO 或者 TextureID 给上层

            // 模拟触发外部回调
            if (self->m_frameCallback) {
                self->m_frameCallback(0 /* fake textureId */, 0 /* fake pts */);
            }
            AImage_delete(image); // 必须归还，否则 ImageReader 卡死
        }
    }

private:
    int m_width = 0, m_height = 0;
    bool m_isPreviewing = false;
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
};

// 平台工厂实现
std::unique_ptr<ICameraCaptureImpl> CreatePlatformCameraImpl() {
    // 根据宏定义或运行时判断实例化对应的实现类
    // 此处我们默认在 Android / Mock 环境实例化 Android 版本
    return std::make_unique<CameraCaptureAndroid>();
}

} // namespace modules
} // namespace video_sdk
