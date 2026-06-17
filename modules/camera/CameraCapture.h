#pragma once
#include <cstdint>
#include <functional>
#include <memory>
#include "core/utils/RenderThread.h"

namespace video_sdk {
namespace modules {

using OnFrameAvailableCallback = std::function<void(uint32_t textureId, int width, int height, int64_t timestampMs, const float* transformMatrix)>;

class ICameraCaptureImpl; // 隐藏平台特定实现

/**
 * @brief 摄像头采集模块 (基于 PIMPL 模式设计)。
 * 采用工业级 "Kotlin 采集 + OES 纹理透传 C++" 架构。
 * 允许 Android/iOS 在系统层完成高兼容性的相机控制，然后将画面纹理传给 C++ 进行极速渲染。
 * 引入 RenderThread 保证外部推送 OES 纹理时的线程安全性。
 */
class CameraCapture {
public:
    CameraCapture();
    ~CameraCapture();

    // 绑定预览的 UI Window (Surface / Layer)
    void setPreviewWindow(void* window);

    // Kotlin/Swift 层接收到相机画面后，将 OES Texture ID 和矩阵传给 C++
    void pushOESTexture(uint32_t textureId, int width, int height, int64_t timestampMs, const float* transformMatrix);

    // 设置帧回调，把 OES 纹理传给下层的 RenderGraph
    void setFrameCallback(OnFrameAvailableCallback callback);

private:
    std::unique_ptr<ICameraCaptureImpl> m_impl;
    std::shared_ptr<core::RenderThread> m_renderThread;
};

// ----------------- PIMPL 内部接口定义 -----------------
class ICameraCaptureImpl {
public:
    virtual ~ICameraCaptureImpl() = default;
    virtual void setPreviewWindow(void* window) = 0;
    virtual void pushOESTexture(uint32_t textureId, int width, int height, int64_t timestampMs, const float* transformMatrix) = 0;
    virtual void setFrameCallback(OnFrameAvailableCallback callback) = 0;
};

} // namespace modules
} // namespace video_sdk
