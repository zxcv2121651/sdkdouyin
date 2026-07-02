#pragma once

#include <string>
#include <unordered_map>
#include <list>
#include <mutex>
#include <memory>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <vector>
#include "hal/interface/IDecoderPool.h"
#include "media/codec/SoftwareVideoDecoder.h"
#include "hal/codec/AndroidMediaCodecDecoder.h"

namespace video_sdk {
namespace hal {

/**
 * @brief 商业级解码器池：实现 LRU 缓存策略 + Pin/Unpin 状态保护 + 异步预读。
 * 解决移动端硬件解码器实例化缓慢及实例数量受限的问题。
 */
class DecoderPool : public IDecoderPool {
public:
    explicit DecoderPool(size_t maxCapacity = 8);
    ~DecoderPool() override;

    void requestDecoder(const std::string& assetPath) override;

    // 获取或创建软件解码器 (用于演示或兼容路径)
    std::shared_ptr<media::SoftwareVideoDecoder> requestSoftwareDecoderInstance(const std::string& assetPath);

    // 获取或创建硬件解码器 (零拷贝路径)
    std::shared_ptr<AndroidMediaCodecDecoder> requestHardwareDecoderInstance(const std::string& assetPath, uint32_t codecId, int width, int height, void* surface);

    // 释放解码器（将其从池中彻底移除并销毁）
    void releaseDecoder(const std::string& assetPath) override;

    // 标记解码器为"正在使用" (Pinned)，防止被 LRU 淘汰
    void pinDecoder(const std::string& assetPath);

    // 解除"正在使用"标记 (Unpinned)，允许被 LRU 淘汰
    void unpinDecoder(const std::string& assetPath);

    // 异步预读硬件解码器
    void preloadHardwareDecoderAsync(const std::string& assetPath, uint32_t codecId, int width, int height, void* surface);

    // 清理所有未 Pinned 的缓存（如响应 Android onTrimMemory）
    void evictAllUnpinned();

private:
    struct CacheEntry {
        std::shared_ptr<media::SoftwareVideoDecoder> softDecoder;
        std::shared_ptr<AndroidMediaCodecDecoder> hwDecoder;
        std::list<std::string>::iterator lruIter;
        bool isPinned = false; // 处于 Pinned 状态的解码器不能被淘汰
    };

    // 尝试淘汰一个未被 Pin 的解码器，如果成功返回 true
    bool evictOneUnpinned_Locked();

    size_t m_maxCapacity;
    std::mutex m_mutex;
    std::list<std::string> m_lruList;
    std::unordered_map<std::string, CacheEntry> m_cache;
};

} // namespace hal
} // namespace video_sdk
