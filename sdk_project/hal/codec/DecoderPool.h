#pragma once

#include <string>
#include <unordered_map>
#include <list>
#include <mutex>
#include <memory>
#include "hal/interface/IDecoderPool.h"
#include "media/codec/SoftwareVideoDecoder.h"
#include "hal/codec/AndroidMediaCodecDecoder.h"

namespace video_sdk {
namespace hal {

/**
 * @brief 解码器池：实现 LRU (Least Recently Used) 缓存策略。
 * 解决移动端硬件解码器实例化缓慢及实例数量受限 (如仅支持16个实例) 的问题。
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

    void releaseDecoder(const std::string& assetPath) override;

private:
    struct CacheEntry {
        std::shared_ptr<media::SoftwareVideoDecoder> softDecoder;
        std::shared_ptr<AndroidMediaCodecDecoder> hwDecoder;
        std::list<std::string>::iterator lruIter;
    };

    size_t m_maxCapacity;
    std::mutex m_mutex;
    std::list<std::string> m_lruList;
    std::unordered_map<std::string, CacheEntry> m_cache;
};

} // namespace hal
} // namespace video_sdk
