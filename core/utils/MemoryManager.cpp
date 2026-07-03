#include "MemoryManager.h"
#include <iostream>
#include <algorithm>

namespace video_sdk {
namespace core {

MemoryManager& MemoryManager::getInstance() {
    static MemoryManager instance;
    return instance;
}

void MemoryManager::addListener(std::weak_ptr<IMemoryPressureListener> listener) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_listeners.push_back(listener);
}

void MemoryManager::removeListener(std::shared_ptr<IMemoryPressureListener> listener) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_listeners.erase(
        std::remove_if(m_listeners.begin(), m_listeners.end(),
            [&listener](const std::weak_ptr<IMemoryPressureListener>& wp) {
                auto sp = wp.lock();
                return !sp || sp == listener;
            }),
        m_listeners.end());
}

void MemoryManager::notifyMemoryPressure(MemoryPressureLevel level) {
    std::vector<std::shared_ptr<IMemoryPressureListener>> activeListeners;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        // 清理已经失效的 weak_ptr，并收集存活的 listener
        auto it = m_listeners.begin();
        while (it != m_listeners.end()) {
            if (auto sp = it->lock()) {
                activeListeners.push_back(sp);
                ++it;
            } else {
                it = m_listeners.erase(it);
            }
        }
    }

    std::cout << "[MemoryManager] Dispatching Memory Pressure Event (Level: " << (int)level
              << ") to " << activeListeners.size() << " listeners." << std::endl;

    // 分发通知，不在锁内执行，防止 listener 内部发生死锁
    for (auto& listener : activeListeners) {
        listener->onMemoryPressure(level);
    }
}

} // namespace core
} // namespace video_sdk
