#include "CameraCapture.h"

namespace video_sdk {
namespace modules {

CameraCapture::CameraCapture() : m_width(1080), m_height(1920), m_fps(30) {}

CameraCapture::~CameraCapture() {
    stopPreview();
}

bool CameraCapture::initialize(int width, int height, int fps) {
    m_width = width;
    m_height = height;
    m_fps = fps;
    // 双端初始化逻辑：
    // Android: JNI 调用 Kotlin 层配置 Camera2
    // iOS: Obj-C++ 调用 AVCaptureSession 配置
    return true;
}

void CameraCapture::startPreview() {
    m_isPreviewing = true;
    // 发送指令开启硬件相机数据流
}

void CameraCapture::stopPreview() {
    m_isPreviewing = false;
    // 停止数据流
}

void CameraCapture::switchCamera() {
    // 发送指令切换前后置镜头
}

void CameraCapture::setFrameCallback(OnFrameAvailableCallback callback) {
    m_frameCallback = callback;
}

} // namespace modules
} // namespace video_sdk
