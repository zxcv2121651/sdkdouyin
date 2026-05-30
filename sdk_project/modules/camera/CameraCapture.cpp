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

bool CameraCapture::initialize(int width, int height, int fps) {
    if (m_impl) return m_impl->initialize(width, height, fps);
    return false;
}

void CameraCapture::startPreview() {
    if (m_impl) m_impl->startPreview();
}

void CameraCapture::stopPreview() {
    if (m_impl) m_impl->stopPreview();
}

void CameraCapture::switchCamera() {
    if (m_impl) m_impl->switchCamera();
}

void CameraCapture::setFrameCallback(OnFrameAvailableCallback callback) {
    if (m_impl) m_impl->setFrameCallback(callback);
}

} // namespace modules
} // namespace video_sdk
