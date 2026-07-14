#include "DefaultFBOPool.h"
#include <iostream>

namespace video_sdk {
namespace core {

DefaultFBOPool::DefaultFBOPool(std::shared_ptr<rhi::IRenderer> renderer, size_t maxCapacity)
    : m_renderer(std::move(renderer)), m_maxCapacity(maxCapacity) {
}

DefaultFBOPool::~DefaultFBOPool() {
    purge();
}

void DefaultFBOPool::init() {
    MemoryManager::getInstance().addListener(weak_from_this());
}

rhi::FrameBufferObject* DefaultFBOPool::acquireFBO(int width, int height) {
    if (!m_renderer) return nullptr;

    std::string key = getSizeKey(width, height);
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_cache.find(key);
    if (it != m_cache.end() && !it->second.empty()) {
        // 命中缓存，直接取出一个复用
        rhi::FrameBufferObject* fbo = it->second.front();
        it->second.pop_front();
        m_currentSize--;
        // std::cout << "[FBOPool] Acquired cached FBO (" << width << "x" << height << "), pool size: " << m_currentSize << std::endl;
        return fbo;
    }

    // 未命中缓存，直接让 RHI 创建一个新的
    // std::cout << "[FBOPool] Creating new FBO (" << width << "x" << height << ")" << std::endl;
    return m_renderer->acquireFBO(width, height);
}

void DefaultFBOPool::releaseFBO(rhi::FrameBufferObject* fbo) {
    if (!fbo || !m_renderer) return;

    std::lock_guard<std::mutex> lock(m_mutex);

    // 如果池子已经满了，直接释放给 GPU
    if (m_currentSize >= m_maxCapacity) {
        // 简单的淘汰策略：超出容量就不回收了
        m_renderer->releaseFBO(fbo);
        return;
    }

    std::string key = getSizeKey(fbo->width, fbo->height);
    m_cache[key].push_back(fbo);
    m_currentSize++;
    // std::cout << "[FBOPool] Recycled FBO (" << fbo->width << "x" << fbo->height << "), pool size: " << m_currentSize << std::endl;
}

void DefaultFBOPool::purge() {
    if (!m_renderer) return;

    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& pair : m_cache) {
        for (rhi::FrameBufferObject* fbo : pair.second) {
            m_renderer->releaseFBO(fbo);
        }
    }
    m_cache.clear();
    m_currentSize = 0;
    std::cout << "[FBOPool] Purged all cached FBOs." << std::endl;
}

void DefaultFBOPool::onMemoryPressure(MemoryPressureLevel level) {
    // 无论是 Moderate 还是 Critical，既然系统警告了，我们直接清空当前没在使用的 FBO 缓存归还显存
    std::cout << "[FBOPool] Handling Memory Pressure Event, purging pool..." << std::endl;
    purge();
}

} // namespace core
} // namespace video_sdk
