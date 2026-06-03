#pragma once
#include <memory>
#include "AVSyncClock.h"
#include "../codec/SoftwareVideoDecoder.h"
#include "core/utils/LockFreeRingBuffer.h"
#include "core/utils/MessageLoop.h"

namespace video_sdk {
namespace media {

/**
 * @brief 渲染线程同步循环引擎。
 * 核心升级：废弃 while+sleep 的轮询架构，改用 Google 风格的 MessageLoop 驱动。
 * 支持精确的时钟定时与异步任务派发。
 */
class RenderThreadSyncLoop {
public:
    RenderThreadSyncLoop(std::shared_ptr<AVSyncClock> masterClock, size_t bufferSize = 10);
    ~RenderThreadSyncLoop();

    void start();
    void stop();

    // 解码线程调用：将解码完成的帧送入无锁渲染队列，并发送一个渲染消息
    bool enqueueFrame(const VideoFrame& frame);

    // 允许外部向渲染线程抛任务 (例如 EGL Context 初始化、切换滤镜等)
    void postTask(std::function<void()> task);

private:
    void scheduleNextRender();
    void renderFrame(const VideoFrame& frame);

private:
    std::shared_ptr<AVSyncClock> m_masterClock;
    core::MessageLoop m_messageLoop;

    core::LockFreeRingBuffer<VideoFrame> m_frameQueue;

    const int64_t SYNC_THRESHOLD_MIN = -15; // 滞后超过15ms考虑丢帧
    const int64_t SYNC_THRESHOLD_MAX = 10;  // 偏早超过10ms进行延迟调度
};

} // namespace media
} // namespace video_sdk
