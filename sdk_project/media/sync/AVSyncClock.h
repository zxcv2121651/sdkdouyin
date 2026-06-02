#pragma once
#include <cstdint>

namespace video_sdk {
namespace media {

/**
 * @brief 工业级音画同步核心机制：主时钟 (Master Clock)。
 *
 * 核心原理：
 * 人耳对音频的卡顿（即使只有 10ms）极其敏感，而人眼对视频画面的掉帧（缺个一两帧）相对宽容。
 * 因此在所有顶级播放器 (如 ExoPlayer / ijkplayer) 中，绝对不允许“音频等视频”。
 *
 * 这个类的作用是锚定“绝对的音频播放进度时间”。
 * 视频渲染线程 (RenderThreadSyncLoop) 每次拿到一帧画面，都会通过 `getClock()` 查询这个主时钟。
 * 画面早了就休眠，画面晚了就果断把画面丢弃，死死咬住这个时钟。
 */
class AVSyncClock {
public:
    AVSyncClock();
    ~AVSyncClock() = default;

    /**
     * @brief 锚定/重置主时钟。
     * 当用户拖动进度条 (Seek) 到第 5 秒时，必须强制将时钟设置为 5000ms。
     * 或者当音频硬件 OpenSL ES 缓冲耗尽并重新开始填填满时，校准该时钟。
     */
    void setClock(int64_t pts);

    /**
     * @brief 获取当前绝对时钟的时间戳 (毫秒)。
     * 在本 Demo 中它基于系统高精度单调时钟推进，但在实际业务中，
     * 它通常通过读取音频设备底层（如 AudioTrack.getPlaybackHeadPosition()）
     * 吐出了多少个 PCM 音频采样率样本，反推计算出精确时间，这样永远不会有音画不同步。
     */
    int64_t getClock() const;

    /**
     * @brief 辅助函数：给定视频的 PTS，计算与主时钟的差距。
     * 返回正数代表视频偏早 (需休眠)，负数代表视频落后 (需可能丢帧)。
     */
    int64_t computeVideoDelay(int64_t videoPts);

private:
    int64_t m_currentPts = 0;        // 锚点基础 PTS
    int64_t m_lastSystemTime = 0;    // 设置锚点时的系统绝对时间
};

} // namespace media
} // namespace video_sdk
