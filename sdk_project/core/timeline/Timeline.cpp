#include "Timeline.h"
#include <algorithm>
#include <mutex>
#include "Clip.h"

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

    // 获取独占写锁 (Writer Lock)
    std::unique_lock<std::shared_mutex> writeLock(m_mutex);

    // 1. O(1) 字典保存
    m_clipMap[clip->getId()] = clip;

    // 2. 插入有序数组，并保证依据 timelineIn 排序
    // 这里使用 std::upper_bound 实现 O(logN) 的插入点查找
    auto it = std::upper_bound(m_orderedClips.begin(), m_orderedClips.end(), clip,
        [](const std::shared_ptr<Clip>& a, const std::shared_ptr<Clip>& b) {
            return a->getTimelineIn() < b->getTimelineIn();
        });

    m_orderedClips.insert(it, clip);
}

std::vector<std::shared_ptr<Clip>> Timeline::getClipsAtTime(int64_t timeMs) {
    std::vector<std::shared_ptr<Clip>> activeClips;

    // 获取共享读锁 (Reader Lock) - 允许渲染线程并发安全读取，不会阻塞彼此
    std::shared_lock<std::shared_mutex> readLock(m_mutex);

    // 工业级 NLE 优化：利用已排序的 m_orderedClips 进行高效查找
    // 相交检测逻辑：
    // 找出所有 timelineIn <= timeMs 的片段中，timelineOut > timeMs 的那部分。

    // 1. 二分查找：找到第一个 timelineIn > timeMs 的片段位置
    // 所有该位置之前的片段，其 timelineIn 必然 <= timeMs
    auto itEnd = std::upper_bound(m_orderedClips.begin(), m_orderedClips.end(), timeMs,
        [](int64_t time, const std::shared_ptr<Clip>& clip) {
            return time < clip->getTimelineIn();
        });

    // 2. 反向遍历从 itEnd 往前找 (因为可能存在多个并行的轨道重叠)
    for (auto it = m_orderedClips.begin(); it != itEnd; ++it) {
        if ((*it)->getTimelineOut() > timeMs) {
            // 片段满足：timelineIn <= timeMs 且 timelineOut > timeMs
            activeClips.push_back(*it);
        }
    }

    return activeClips;
}

} // namespace core
} // namespace video_sdk
