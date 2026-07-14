#pragma once
#include "core/include/IFBOPool.h"
#include "core/utils/MemoryManager.h"
#include "rhi/interface/IRenderer.h"
#include <mutex>
#include <list>
#include <unordered_map>
#include <string>
#include <memory>

namespace video_sdk {
namespace core {

/**
 * @brief 默认的 FBO 内存池实现，继承自 IMemoryPressureListener
 * 基于尺寸(Width x Height) 缓存 FBO，支持 LRU 淘汰和全局内存告警响应。
 */
class DefaultFBOPool : public IFBOPool, public IMemoryPressureListener, public std::enable_shared_from_this<DefaultFBOPool> {
public:
    explicit DefaultFBOPool(std::shared_ptr<rhi::IRenderer> renderer, size_t maxCapacity = 20);
    ~DefaultFBOPool() override;

    // 必须在 shared_ptr 初始化后调用，以注册到 MemoryManager
    void init();

    rhi::FrameBufferObject* acquireFBO(int width, int height) override;
    void releaseFBO(rhi::FrameBufferObject* fbo) override;
    void purge() override;

    // IMemoryPressureListener 实现
    void onMemoryPressure(MemoryPressureLevel level) override;

private:
    std::shared_ptr<rhi::IRenderer> m_renderer;
    size_t m_maxCapacity;
    std::mutex m_mutex;

    // Key: "widthxheight", Value: 对应的 FBO 列表
    std::unordered_map<std::string, std::list<rhi::FrameBufferObject*>> m_cache;

    // 记录 FBO 的总数量，用于触发 LRU 淘汰
    size_t m_currentSize = 0;

    std::string getSizeKey(int width, int height) const {
        return std::to_string(width) + "x" + std::to_string(height);
    }
};

} // namespace core
} // namespace video_sdk
