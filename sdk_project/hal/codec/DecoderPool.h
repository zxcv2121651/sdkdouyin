#pragma once

#include "../interface/IDecoderPool.h"
#include <string>
#include <list>
#include <unordered_map>
#include <memory>
#include <mutex>
#include "media/codec/SoftwareVideoDecoder.h"

namespace video_sdk {
namespace hal {

/**
 * @brief 工业级解码器缓存池 (基于 LRU 策略)。
 * 在复杂的 NLE (非编) 场景中，同一时刻可能需要大量解码器实例。
 * 为防止 OOM (Out Of Memory) 或者超过 Android MediaCodec 硬件实例数限制，
 * 我们通过 LRU (Least Recently Used) 算法严格管控存活的解码器。
 */
class DecoderPool : public IDecoderPool {
public:
    // 默认最多允许 3 个并发视频解码器存活
    explicit DecoderPool(size_t maxCapacity = 3);
    ~DecoderPool() override;

    // 请求一个解码器（如果池子中存在且可用，则直接返回；否则创建新的；若超容则踢掉最老的）
    std::shared_ptr<media::SoftwareVideoDecoder> requestDecoderInstance(const std::string& assetPath);

    void requestDecoder(const std::string& assetPath) override;
    void releaseDecoder(const std::string& assetPath) override;

private:
    size_t m_maxCapacity;
    std::mutex m_mutex;

    // LRU 双向链表，记录最近使用的顺序 (Front 是最新使用的)
    std::list<std::string> m_lruList;

    // 映射表：assetPath -> (Decoder实例, 链表迭代器)
    struct PoolItem {
        std::shared_ptr<media::SoftwareVideoDecoder> decoder;
        std::list<std::string>::iterator lruIter;
    };
    std::unordered_map<std::string, PoolItem> m_cache;
};

} // namespace hal
} // namespace video_sdk
