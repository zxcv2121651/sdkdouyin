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
}

CameraCapture::~CameraCapture() = default;

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

void CameraCapture::setPreviewSurface(void* nativeWindow) {
    if (m_impl) m_impl->setPreviewSurface(nativeWindow);
}

void CameraCapture::startRecording(const std::string& outputPath) {
    if (m_impl) m_impl->startRecording(outputPath);
}

void CameraCapture::stopRecording() {
    if (m_impl) m_impl->stopRecording();
}

} // namespace modules
} // namespace video_sdk
