#include "DefaultFBOPool.h"
#include <iostream>
#include <algorithm>

namespace video_sdk {
namespace rhi {

DefaultFBOPool::DefaultFBOPool(std::shared_ptr<IRenderer> renderer) : m_renderer(renderer) {
    std::cout << "[FBOPool] Initialized." << std::endl;
}

DefaultFBOPool::~DefaultFBOPool() {
    purge();
}

FrameBufferObject* DefaultFBOPool::acquireFBO(int width, int height) {
    std::lock_guard<std::mutex> lock(m_mutex);
    FBOKey key{width, height};

    auto it = m_freeFBOs.find(key);
    if (it != m_freeFBOs.end() && !it->second.empty()) {
        FrameBufferObject* fbo = it->second.back();
        it->second.pop_back();
        m_lruList.remove(fbo);
        return fbo;
    }

    if (m_renderer) {
        auto fbo = m_renderer->acquireFBO(width, height);
        std::cout << "[FBOPool] Cache MISS. Allocated new FBO: " << width << "x" << height << " id:" << (fbo ? fbo->fboId : 0) << std::endl;
        return fbo;
    }

    return nullptr;
}

void DefaultFBOPool::recycleFBO(FrameBufferObject* fbo) {
    if (!fbo) return;

    std::lock_guard<std::mutex> lock(m_mutex);
    FBOKey key{fbo->width, fbo->height};

    m_freeFBOs[key].push_back(fbo);
    m_lruList.push_front(fbo);

    if (m_lruList.size() > m_maxPoolSize) {
        auto lruFbo = m_lruList.back();
        m_lruList.pop_back();

        FBOKey lruKey{lruFbo->width, lruFbo->height};
        auto& list = m_freeFBOs[lruKey];
        auto listIt = std::find(list.begin(), list.end(), lruFbo);
        if (listIt != list.end()) {
            list.erase(listIt);
        }

        if (m_renderer) {
            m_renderer->releaseFBO(lruFbo);
        }
        std::cout << "[FBOPool] Evicted FBO " << lruFbo->fboId << " from pool (Max capacity " << m_maxPoolSize << " reached)." << std::endl;
    }
}

void DefaultFBOPool::purge() {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto fbo : m_lruList) {
        if (m_renderer) m_renderer->releaseFBO(fbo);
    }
    m_lruList.clear();
    m_freeFBOs.clear();
    std::cout << "[FBOPool] Purged all cached FBOs." << std::endl;
}

}
}
