#pragma once
#include <cstdint>
#include <functional>

namespace video_sdk {
namespace modules {

/**
 * @brief 视频帧回调定义。
 * 采集到的纹理ID或YUV数据通过该回调传递给渲染引擎进行美颜或直接录制。
 */
using OnFrameAvailableCallback = std::function<void(uint32_t textureId, int64_t timestampMs)>;

/**
 * @brief 极速录制采集模块。
 * 负责与双端的底层硬件 API 交互（如 Android 的 Camera2/SurfaceTexture，iOS 的 AVCaptureSession/CVPixelBuffer）。
 */
class CameraCapture {
public:
    CameraCapture();
    ~CameraCapture();

    // 初始化相机参数
    bool initialize(int width, int height, int fps);

    // 开始采集与预览
    void startPreview();

    // 停止采集
    void stopPreview();

    // 切换前后摄像头
    void switchCamera();

    // 设置帧回调
    void setFrameCallback(OnFrameAvailableCallback callback);

private:
    int m_width;
    int m_height;
    int m_fps;
    bool m_isPreviewing = false;
    OnFrameAvailableCallback m_frameCallback;
};

} // namespace modules
} // namespace video_sdk
