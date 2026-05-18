#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <memory>
#include "AVSyncClock.h"
#include "../codec/SoftwareVideoDecoder.h"

namespace video_sdk {
namespace media {

/**
 * @brief 渲染线程同步循环引擎。
 * 负责接收解码后的 VideoFrame，依据 AVSyncClock 的时间戳对比决定：
 * 渲染(Render)、休眠等待(Wait) 或 丢帧(Drop)。
 */
class RenderThreadSyncLoop {
public:
    RenderThreadSyncLoop(std::shared_ptr<AVSyncClock> masterClock);
    ~RenderThreadSyncLoop();

    // 启动渲染循环线程
    void start();

    // 停止渲染循环
    void stop();

    // 外部解码器将解码完成的帧送入渲染队列
    void enqueueFrame(const VideoFrame& frame);

private:
    void threadLoop();
    void renderFrame(const VideoFrame& frame);

private:
    std::shared_ptr<AVSyncClock> m_masterClock;

    std::thread m_renderThread;
    std::atomic<bool> m_isRunning{false};

    std::mutex m_queueMutex;
    std::condition_variable m_queueCondVar;
    std::queue<VideoFrame> m_frameQueue;

    // 视频同步容差阈值 (毫秒)
    const int64_t SYNC_THRESHOLD_MIN = -15; // 滞后超过15ms考虑丢帧
    const int64_t SYNC_THRESHOLD_MAX = 10;  // 偏早超过10ms进行休眠
};

} // namespace media
} // namespace video_sdk
