#include "Timeline.h"
#include <algorithm>
#include <mutex>

namespace video_sdk {
namespace core {

Timeline::Timeline() {}

Timeline::~Timeline() {
    std::unique_lock<std::shared_mutex> writeLock(m_mutex);
    m_clipMap.clear();
    m_orderedClips.clear();
}

void Timeline::addClip(const std::string& trackId, std::shared_ptr<Clip> clip) {
    if (!clip) return;

    std::unique_lock<std::shared_mutex> writeLock(m_mutex);
    m_clipMap[clip->getId()] = clip;

    auto it = std::upper_bound(m_orderedClips.begin(), m_orderedClips.end(), clip,
        [](const std::shared_ptr<Clip>& a, const std::shared_ptr<Clip>& b) {
            return a->getTimelineIn() < b->getTimelineIn();
        });

    m_orderedClips.insert(it, clip);
}

std::vector<std::shared_ptr<Clip>> Timeline::getClipsAtTime(int64_t timeMs) {
    std::vector<std::shared_ptr<Clip>> activeClips;
    std::shared_lock<std::shared_mutex> readLock(m_mutex);

    auto itEnd = std::upper_bound(m_orderedClips.begin(), m_orderedClips.end(), timeMs,
        [](int64_t time, const std::shared_ptr<Clip>& clip) {
            return time < clip->getTimelineIn();
        });

    for (auto it = m_orderedClips.begin(); it != itEnd; ++it) {
        if ((*it)->getTimelineOut() > timeMs) {
            activeClips.push_back(*it);
        }
    }

    return activeClips;
}

void Timeline::removeClip(const std::string& clipId) {
    std::unique_lock<std::shared_mutex> writeLock(m_mutex);
    auto it = m_clipMap.find(clipId);
    if (it != m_clipMap.end()) {
        auto clip = it->second;
        m_clipMap.erase(it);
        m_orderedClips.erase(
            std::remove(m_orderedClips.begin(), m_orderedClips.end(), clip),
            m_orderedClips.end()
        );
    }
}

void Timeline::updateClipRange(const std::string& clipId, int64_t newInMs, int64_t newOutMs) {
    std::unique_lock<std::shared_mutex> writeLock(m_mutex);
    auto it = m_clipMap.find(clipId);
    if (it != m_clipMap.end()) {
        auto clip = it->second;
        clip->setTimelineRange(newInMs, newOutMs);
        std::sort(m_orderedClips.begin(), m_orderedClips.end(),
            [](const std::shared_ptr<Clip>& a, const std::shared_ptr<Clip>& b) {
                return a->getTimelineIn() < b->getTimelineIn();
            });
    }
}

std::shared_ptr<Clip> Timeline::getClip(const std::string& clipId) {
    std::shared_lock<std::shared_mutex> readLock(m_mutex);
    auto it = m_clipMap.find(clipId);
    if (it != m_clipMap.end()) {
        return it->second;
    }
    return nullptr;
}

} // namespace core
} // namespace video_sdk
