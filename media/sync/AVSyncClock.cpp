#include "AVSyncClock.h"
#include <chrono>

namespace video_sdk {
namespace media {

// 获取当前系统时间的辅助函数
static int64_t getCurrentTimeMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
}

AVSyncClock::AVSyncClock() {
    setClock(0);
}

void AVSyncClock::setClock(int64_t pts) {
    m_currentPts = pts;
    m_lastSystemTime = getCurrentTimeMs();
}

int64_t AVSyncClock::getClock() const {
    // 主时钟 = 上一次设置的 PTS + 系统流逝的时间
    int64_t currentTime = getCurrentTimeMs();
    return m_currentPts + (currentTime - m_lastSystemTime);
}

int64_t AVSyncClock::computeVideoDelay(int64_t videoPts) {
    int64_t masterClock = getClock();
    int64_t diff = videoPts - masterClock;

    // 假设 A/V 偏差阈值为 10ms
    // 差值 diff > 0: 视频偏早，需要 wait 延迟渲染
    // 差值 diff < 0: 视频滞后，如果滞后过多可能需要告诉外部直接丢帧
    return diff;
}

} // namespace media
} // namespace video_sdk
