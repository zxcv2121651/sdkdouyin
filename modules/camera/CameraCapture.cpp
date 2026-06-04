#include "CameraCapture.h"

// 声明外部的平台特定工厂函数
namespace video_sdk {
namespace modules {
extern std::unique_ptr<ICameraCaptureImpl> CreatePlatformCameraImpl();
}
}

namespace video_sdk {
namespace modules {

CameraCapture::CameraCapture() {
    m_impl = CreatePlatformCameraImpl();
}

CameraCapture::~CameraCapture() = default;

void CameraCapture::setPreviewWindow(void* window) {
    if (m_impl) m_impl->setPreviewWindow(window);
}

void CameraCapture::pushOESTexture(uint32_t textureId, int width, int height, int64_t timestampMs, const float* transformMatrix) {
    if (m_impl) m_impl->pushOESTexture(textureId, width, height, timestampMs, transformMatrix);
}

void CameraCapture::setFrameCallback(OnFrameAvailableCallback callback) {
    if (m_impl) m_impl->setFrameCallback(callback);
}

} // namespace modules
} // namespace video_sdk
