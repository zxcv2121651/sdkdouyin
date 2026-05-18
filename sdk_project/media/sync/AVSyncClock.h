#pragma once
#include <cstdint>

namespace video_sdk {
namespace media {

/**
 * @brief 音视频同步时钟。
 * 工业级播放器通常以音频时间戳 (Audio PTS) 作为主时钟 (Master Clock)。
 * 视频帧的渲染需要通过对比本时钟来决定是丢帧 (Drop)、立即渲染 (Render) 还是等待 (Wait)。
 */
class AVSyncClock {
public:
    AVSyncClock();
    ~AVSyncClock() = default;

    // 设置初始时钟或发生 Seek 时重置时钟
    void setClock(int64_t pts);

    // 获取当前主时钟的时间（通常基于系统时间推进，或者跟随音频回调推进）
    int64_t getClock() const;

    // 视频帧到达时，计算与主时钟的偏差，返回需要延迟渲染的毫秒数
    // 如果返回值为负数，表示视频滞后，可能需要丢帧。
    int64_t computeVideoDelay(int64_t videoPts);

private:
    int64_t m_currentPts = 0;
    int64_t m_lastSystemTime = 0;
};

} // namespace media
} // namespace video_sdk
