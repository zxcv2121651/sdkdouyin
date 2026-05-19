#include "DecoderPool.h"
#include <iostream>

namespace video_sdk {
namespace hal {

DecoderPool::DecoderPool(size_t maxCapacity) : m_maxCapacity(maxCapacity) {}

DecoderPool::~DecoderPool() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cache.clear();
    m_lruList.clear();
}

void DecoderPool::requestDecoder(const std::string& assetPath) {
    // IDecoderPool 抽象接口实现，通常用作预热
    requestDecoderInstance(assetPath);
}

std::shared_ptr<media::SoftwareVideoDecoder> DecoderPool::requestDecoderInstance(const std::string& assetPath) {
    std::lock_guard<std::mutex> lock(m_mutex);

    // 1. 如果已经在池中，更新 LRU 并返回
    auto it = m_cache.find(assetPath);
    if (it != m_cache.end()) {
        std::cout << "[DecoderPool] Cache HIT for: " << assetPath << std::endl;
        // 移到链表头部 (最近使用)
        m_lruList.erase(it->second.lruIter);
        m_lruList.push_front(assetPath);
        it->second.lruIter = m_lruList.begin();
        return it->second.decoder;
    }

    std::cout << "[DecoderPool] Cache MISS for: " << assetPath << ". Creating new decoder." << std::endl;

    // 2. 如果池已满，执行 LRU 驱逐策略
    if (m_cache.size() >= m_maxCapacity) {
        std::string oldestAsset = m_lruList.back();
        std::cout << "[DecoderPool] Pool FULL. Evicting oldest decoder: " << oldestAsset << std::endl;
        m_lruList.pop_back();

        // 销毁旧解码器
        m_cache[oldestAsset].decoder->destroy();
        m_cache.erase(oldestAsset);
    }

    // 3. 创建并初始化新解码器
    auto newDecoder = std::make_shared<media::SoftwareVideoDecoder>();
    // 这里使用假 CodecID 进行演示初始化
    newDecoder->initialize(27); // 27 = AV_CODEC_ID_H264

    // 放入池中 (链表头部)
    m_lruList.push_front(assetPath);
    m_cache[assetPath] = {newDecoder, m_lruList.begin()};

    return newDecoder;
}

void DecoderPool::releaseDecoder(const std::string& assetPath) {
    std::lock_guard<std::mutex> lock(m_mutex);

    // 我们通常不主动销毁，而是依靠 LRU 淘汰机制。
    // 但如果业务明确要求强制释放（比如用户删除了视频段），可以在这里主动剔除
    auto it = m_cache.find(assetPath);
    if (it != m_cache.end()) {
        std::cout << "[DecoderPool] Explicitly releasing decoder for: " << assetPath << std::endl;
        m_lruList.erase(it->second.lruIter);
        it->second.decoder->destroy();
        m_cache.erase(it);
    }
}

} // namespace hal
} // namespace video_sdk
