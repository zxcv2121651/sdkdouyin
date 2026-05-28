#pragma once
#include <thread>
#include <atomic>
#include <memory>
#include "AVSyncClock.h"
#include "../codec/SoftwareVideoDecoder.h"
#include "core/utils/LockFreeRingBuffer.h"

namespace video_sdk {
namespace media {

/**
 * @brief 渲染线程同步循环引擎。
 * 核心升级：不再使用带锁的 std::queue，而是使用 LockFreeRingBuffer
 * 实现极低延迟的解码-渲染数据传递。
 */
class RenderThreadSyncLoop {
public:
    // 默认给 RingBuffer 分配 10 帧的缓冲空间
    RenderThreadSyncLoop(std::shared_ptr<AVSyncClock> masterClock, size_t bufferSize = 10);
    ~RenderThreadSyncLoop();

    // 启动渲染循环线程
    void start();

    // 停止渲染循环
    void stop();

    // 解码线程调用：将解码完成的帧送入无锁渲染队列
    // 如果队列满了，会返回 false，解码线程可以稍作休眠重试 (自旋锁概念)
    bool enqueueFrame(const VideoFrame& frame);

private:
    void threadLoop();
    void renderFrame(const VideoFrame& frame);

private:
    std::shared_ptr<AVSyncClock> m_masterClock;

    std::thread m_renderThread;
    std::atomic<bool> m_isRunning{false};

    // 工业级替换：基于 C++11 std::atomic 的无锁环形队列
    core::LockFreeRingBuffer<VideoFrame> m_frameQueue;

    // 视频同步容差阈值 (毫秒)
    const int64_t SYNC_THRESHOLD_MIN = -15; // 滞后超过15ms考虑丢帧
    const int64_t SYNC_THRESHOLD_MAX = 10;  // 偏早超过10ms进行休眠
};

} // namespace media
} // namespace video_sdk
