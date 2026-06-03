#include "Clip.h"
#include <algorithm>

namespace video_sdk {
namespace core {

Clip::Clip(const std::string& id, const std::string& sourcePath)
    : m_id(id), m_sourcePath(sourcePath) {}

void Clip::setTimelineRange(int64_t inMs, int64_t outMs) {
    m_timelineIn = inMs;
    m_timelineOut = outMs;
}

void Clip::setSourceRange(int64_t inMs, int64_t outMs) {
    m_sourceIn = inMs;
    m_sourceOut = outMs;
}

bool Clip::containsTimelineTime(int64_t timelineTimeMs) const {
    return timelineTimeMs >= m_timelineIn && timelineTimeMs < m_timelineOut;
}

int64_t Clip::mapTimelineToSourceTime(int64_t timelineTimeMs) const {
    if (!containsTimelineTime(timelineTimeMs)) {
        return -1; // 不在此片段内
    }
    // 线性映射：偏移量 = timelineTimeMs - m_timelineIn
    // 这里没有考虑变速(Speed)属性，如果支持变速需要乘以变速系数
    int64_t offset = timelineTimeMs - m_timelineIn;
    return m_sourceIn + offset;
}

} // namespace core
} // namespace video_sdk
