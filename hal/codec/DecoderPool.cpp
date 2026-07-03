#include "DecoderPool.h"
#include <iostream>
#include <future>

namespace video_sdk {
namespace hal {

DecoderPool::DecoderPool(size_t maxCapacity) : m_maxCapacity(maxCapacity) {}

DecoderPool::~DecoderPool() {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& pair : m_cache) {
        if (pair.second.softDecoder) pair.second.softDecoder->destroy();
        if (pair.second.hwDecoder) pair.second.hwDecoder->destroy();
    }
    m_cache.clear();
    m_lruList.clear();
}

void DecoderPool::requestDecoder(const std::string& assetPath) {
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
        if (!evictOneUnpinned_Locked()) {
            std::cerr << "[DecoderPool] Capacity reached and no unpinned decoders available to evict!" << std::endl;
            // 商用方案这里可以选择等待，或者强制淘汰，这里为了防止死锁，直接返回 nullptr 或强制扩展
            return nullptr;
        }
    }

    auto newDecoder = std::make_shared<media::SoftwareVideoDecoder>();
    newDecoder->initialize(27); // 假定 27 为 H264
    m_lruList.push_front(assetPath);
    m_cache[assetPath] = {newDecoder, nullptr, m_lruList.begin(), false};

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
         if (!evictOneUnpinned_Locked()) {
            std::cerr << "[DecoderPool] Capacity reached and no unpinned HW decoders to evict!" << std::endl;
            return nullptr;
        }
    }

    auto newDecoder = std::make_shared<AndroidMediaCodecDecoder>();
    // 同步初始化比较耗时，预读方案应调用 preloadHardwareDecoderAsync
    newDecoder->initialize(codecId, width, height, surface);

    m_lruList.push_front(assetPath);
    m_cache[assetPath] = {nullptr, newDecoder, m_lruList.begin(), false};

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

void DecoderPool::pinDecoder(const std::string& assetPath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_cache.find(assetPath);
    if (it != m_cache.end()) {
        it->second.isPinned = true;
    }
}

void DecoderPool::unpinDecoder(const std::string& assetPath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_cache.find(assetPath);
    if (it != m_cache.end()) {
        it->second.isPinned = false;
    }
}

bool DecoderPool::evictOneUnpinned_Locked() {
    // 从 LRU 的尾部（最旧）向头部查找第一个 unpinned 的实例
    for (auto it = m_lruList.rbegin(); it != m_lruList.rend(); ++it) {
        const std::string& assetPath = *it;
        auto cacheIt = m_cache.find(assetPath);
        if (cacheIt != m_cache.end() && !cacheIt->second.isPinned) {
            // 找到了，将其淘汰
            if (cacheIt->second.softDecoder) cacheIt->second.softDecoder->destroy();
            if (cacheIt->second.hwDecoder) cacheIt->second.hwDecoder->destroy();
            m_lruList.erase(cacheIt->second.lruIter);
            m_cache.erase(cacheIt);
            return true;
        }
    }
    return false;
}

void DecoderPool::evictAllUnpinned() {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_lruList.begin();
    while (it != m_lruList.end()) {
        auto cacheIt = m_cache.find(*it);
        if (cacheIt != m_cache.end() && !cacheIt->second.isPinned) {
             if (cacheIt->second.softDecoder) cacheIt->second.softDecoder->destroy();
             if (cacheIt->second.hwDecoder) cacheIt->second.hwDecoder->destroy();
             m_cache.erase(cacheIt);
             it = m_lruList.erase(it);
        } else {
             ++it;
        }
    }
}

void DecoderPool::preloadHardwareDecoderAsync(const std::string& assetPath, uint32_t codecId, int width, int height, void* surface) {
    // 使用 std::async 在后台线程中进行实例化（真实商业SDK中应投递到专用的 IO MessageLoop 线程池中）
    std::thread([this, assetPath, codecId, width, height, surface]() {
        // 先检查是否已经存在
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_cache.find(assetPath) != m_cache.end()) {
                return; // 已经在池里了
            }
            if (m_cache.size() >= m_maxCapacity) {
                if (!evictOneUnpinned_Locked()) {
                    return; // 满了且无法淘汰，放弃预读
                }
            }
            // 先占位，避免重复预读
            m_lruList.push_front(assetPath);
            m_cache[assetPath] = {nullptr, nullptr, m_lruList.begin(), false};
        }

        auto newDecoder = std::make_shared<AndroidMediaCodecDecoder>();
        // 耗时的初始化在后台线程执行
        newDecoder->initialize(codecId, width, height, surface);

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto it = m_cache.find(assetPath);
            if (it != m_cache.end()) {
                it->second.hwDecoder = newDecoder;
            } else {
                // 如果在初始化期间被外部强制移除了
                newDecoder->destroy();
            }
        }
    }).detach();
}

} // namespace hal

namespace hal {
void DecoderPool::init() {
    core::MemoryManager::getInstance().addListener(weak_from_this());
}

void DecoderPool::onMemoryPressure(core::MemoryPressureLevel level) {
    std::cout << "[DecoderPool] Handling Memory Pressure Event, evicting all unpinned decoders..." << std::endl;
    evictAllUnpinned();
}

} // namespace hal
} // namespace video_sdk
