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
    // 默认请求软解以向后兼容
    requestSoftwareDecoderInstance(assetPath);
}

std::shared_ptr<media::SoftwareVideoDecoder> DecoderPool::requestSoftwareDecoderInstance(const std::string& assetPath) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_cache.find(assetPath);
    if (it != m_cache.end() && it->second.softDecoder) {
        m_lruList.erase(it->second.lruIter);
        m_lruList.push_front(assetPath);
        it->second.lruIter = m_lruList.begin();
        return it->second.softDecoder;
    }

    if (m_cache.size() >= m_maxCapacity) {
        std::string oldestAsset = m_lruList.back();
        m_lruList.pop_back();
        if (m_cache[oldestAsset].softDecoder) m_cache[oldestAsset].softDecoder->destroy();
        if (m_cache[oldestAsset].hwDecoder) m_cache[oldestAsset].hwDecoder->destroy();
        m_cache.erase(oldestAsset);
    }

    auto newDecoder = std::make_shared<media::SoftwareVideoDecoder>();
    newDecoder->initialize(27); // AV_CODEC_ID_H264
    m_lruList.push_front(assetPath);
    m_cache[assetPath] = {newDecoder, nullptr, m_lruList.begin()};

    return newDecoder;
}

std::shared_ptr<AndroidMediaCodecDecoder> DecoderPool::requestHardwareDecoderInstance(const std::string& assetPath, uint32_t codecId, int width, int height, void* surface) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_cache.find(assetPath);
    if (it != m_cache.end() && it->second.hwDecoder) {
        m_lruList.erase(it->second.lruIter);
        m_lruList.push_front(assetPath);
        it->second.lruIter = m_lruList.begin();
        return it->second.hwDecoder;
    }

    if (m_cache.size() >= m_maxCapacity) {
        std::string oldestAsset = m_lruList.back();
        m_lruList.pop_back();
        if (m_cache[oldestAsset].softDecoder) m_cache[oldestAsset].softDecoder->destroy();
        if (m_cache[oldestAsset].hwDecoder) m_cache[oldestAsset].hwDecoder->destroy();
        m_cache.erase(oldestAsset);
    }

    auto newDecoder = std::make_shared<AndroidMediaCodecDecoder>();
    newDecoder->initialize(codecId, width, height, surface);

    m_lruList.push_front(assetPath);
    m_cache[assetPath] = {nullptr, newDecoder, m_lruList.begin()};

    return newDecoder;
}

void DecoderPool::releaseDecoder(const std::string& assetPath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_cache.find(assetPath);
    if (it != m_cache.end()) {
        m_lruList.erase(it->second.lruIter);
        if (it->second.softDecoder) it->second.softDecoder->destroy();
        if (it->second.hwDecoder) it->second.hwDecoder->destroy();
        m_cache.erase(it);
    }
}

} // namespace hal
} // namespace video_sdk
