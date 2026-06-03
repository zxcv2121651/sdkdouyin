#pragma once
#include <cstdint>
#include <functional>
#include <memory>

namespace video_sdk {
namespace modules {

using OnFrameAvailableCallback = std::function<void(uint32_t textureId, int64_t timestampMs)>;

class ICameraCaptureImpl; // 隐藏平台特定实现

/**
 * @brief 极速录制采集模块 (基于 PIMPL 模式设计)。
 * 隐藏底层复杂的 NDK Camera2 / iOS AVFoundation 逻辑，保证业务层头文件纯净。
 */
class CameraCapture {
public:
    CameraCapture();
    ~CameraCapture();

    // 绑定预览的 UI Window (Surface / Layer)
    void setPreviewWindow(void* window);

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
    std::unique_ptr<ICameraCaptureImpl> m_impl;
};

// ----------------- PIMPL 内部接口定义 -----------------
class ICameraCaptureImpl {
public:
    virtual ~ICameraCaptureImpl() = default;
    virtual void setPreviewWindow(void* window) = 0;
    virtual bool initialize(int width, int height, int fps) = 0;
    virtual void startPreview() = 0;
    virtual void stopPreview() = 0;
    virtual void switchCamera() = 0;
    virtual void setFrameCallback(OnFrameAvailableCallback callback) = 0;
};

} // namespace modules
} // namespace video_sdk
