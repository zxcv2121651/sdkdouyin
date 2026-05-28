#pragma once
#include <cstdint>
#include <functional>
#include <memory>
#include <string>

namespace video_sdk {
namespace modules {

using OnFrameAvailableCallback = std::function<void(uint32_t textureId, int64_t timestampMs)>;

class ICameraCaptureImpl;

/**
 * @brief 极速录制采集模块 (基于 PIMPL 模式设计)。
 */
class CameraCapture {
public:
    CameraCapture();
    ~CameraCapture();

    bool initialize(int width, int height, int fps);
    void startPreview();
    void stopPreview();
    void switchCamera();
    void setFrameCallback(OnFrameAvailableCallback callback);

    // 新增 Android 独占接口：设置预览窗口和录制
    void setPreviewSurface(void* nativeWindow);
    void startRecording(const std::string& outputPath);
    void stopRecording();

private:
    std::unique_ptr<ICameraCaptureImpl> m_impl;
};

class ICameraCaptureImpl {
public:
    virtual ~ICameraCaptureImpl() = default;
    virtual bool initialize(int width, int height, int fps) = 0;
    virtual void startPreview() = 0;
    virtual void stopPreview() = 0;
    virtual void switchCamera() = 0;
    virtual void setFrameCallback(OnFrameAvailableCallback callback) = 0;

    // 扩展内部接口
    virtual void setPreviewSurface(void* nativeWindow) {}
    virtual void startRecording(const std::string& outputPath) {}
    virtual void stopRecording() {}
};

} // namespace modules
} // namespace video_sdk
