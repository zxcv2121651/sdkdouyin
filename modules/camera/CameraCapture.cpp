#include "CameraCapture.h"

namespace video_sdk {
namespace modules {
extern std::unique_ptr<ICameraCaptureImpl> CreatePlatformCameraImpl();
}
}

namespace video_sdk {
namespace modules {

CameraCapture::CameraCapture() {
    m_impl = CreatePlatformCameraImpl();
    m_renderThread = std::make_shared<core::RenderThread>();
    m_renderThread->startRenderThread();
}

CameraCapture::~CameraCapture() {
    if (m_renderThread) {
        m_renderThread->stopRenderThread();
    }
}

void CameraCapture::setPreviewWindow(void* window) {
    if (m_impl && m_renderThread) {
        m_renderThread->postTask([this, window]() {
            m_impl->setPreviewWindow(window);
        });
    }
}

void CameraCapture::pushOESTexture(uint32_t textureId, int width, int height, int64_t timestampMs, const float* transformMatrix) {
    // 这个接口通常被 Kotlin 主线程或者 Camera Callback 线程极高频调用 (30fps)
    // 必须把它丢到统一的 RenderThread 里去处理，否则直接丢进 OpenGL 会 Crash。
    if (m_impl && m_renderThread) {
        // 深拷贝 float 矩阵，防止出作用域失效
        std::vector<float> matrixCopy(transformMatrix, transformMatrix + 16);

        m_renderThread->postTask([this, textureId, width, height, timestampMs, matrixCopy]() {
            m_impl->pushOESTexture(textureId, width, height, timestampMs, matrixCopy.data());
        });
    }
}

void CameraCapture::setFrameCallback(OnFrameAvailableCallback callback) {
    if (m_impl) m_impl->setFrameCallback(callback);
}

} // namespace modules
} // namespace video_sdk
