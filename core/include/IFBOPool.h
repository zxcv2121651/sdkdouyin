#pragma once
#include "rhi/interface/IRenderer.h"
#include <memory>

namespace video_sdk {
namespace core {

/**
 * @brief 帧缓冲对象池接口。
 * 供 RenderGraph 等引擎模块统一申请和释放 FBO，避免高频显存分配。
 */
class IFBOPool {
public:
    virtual ~IFBOPool() = default;

    // 向池中申请一个符合宽高的 FBO。如果池中没有，则调用 RHI 创建。
    virtual rhi::FrameBufferObject* acquireFBO(int width, int height) = 0;

    // 释放(归还) FBO 到池中供后续复用。
    virtual void releaseFBO(rhi::FrameBufferObject* fbo) = 0;

    // 强制清理池中所有未被使用的 FBO（通常响应内存压力）
    virtual void purge() = 0;
};

} // namespace core
} // namespace video_sdk
