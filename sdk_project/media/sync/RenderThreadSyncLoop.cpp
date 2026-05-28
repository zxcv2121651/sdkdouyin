#include "RenderThreadSyncLoop.h"
#include <iostream>
#include <chrono>

namespace video_sdk {
namespace media {

RenderThreadSyncLoop::RenderThreadSyncLoop(std::shared_ptr<AVSyncClock> masterClock, size_t bufferSize)
    : m_masterClock(masterClock), m_frameQueue(bufferSize) {
}

RenderThreadSyncLoop::~RenderThreadSyncLoop() {
    stop();
}

void RenderThreadSyncLoop::start() {
    if (m_isRunning) return;
    m_isRunning = true;
    m_renderThread = std::thread(&RenderThreadSyncLoop::threadLoop, this);
}

void RenderThreadSyncLoop::stop() {
    if (!m_isRunning) return;
    m_isRunning = false;
    if (m_renderThread.joinable()) {
        m_renderThread.join();
    }
}

bool RenderThreadSyncLoop::enqueueFrame(const VideoFrame& frame) {
    // 生产者调用：非阻塞
    return m_frameQueue.push(frame);
}

void RenderThreadSyncLoop::threadLoop() {
    while (m_isRunning) {
        VideoFrame currentFrame;

        // 消费者调用：非阻塞轮询 (实际中可以通过增加 EventFd 或 条件变量在空时休眠，
        // 这里为了极致的低延迟，采用类似自旋的机制，每次空闲 yield)
        if (!m_frameQueue.pop(currentFrame)) {
            // 队列为空，释放 CPU 时间片
            std::this_thread::yield();
            // 如果不想消耗过多 CPU，可以短暂 sleep_for(1ms)
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        // --- 核心音视频同步逻辑 (A/V Sync) ---
        int64_t delayMs = m_masterClock->computeVideoDelay(currentFrame.pts);

        if (delayMs > SYNC_THRESHOLD_MAX) {
            // 视频偏早 (Video is early)
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
            renderFrame(currentFrame);
        } else if (delayMs < SYNC_THRESHOLD_MIN) {
            // 视频滞后 (Video is late) -> 丢弃
            std::cout << "[LockFree A/V Sync] Video PTS " << currentFrame.pts
                      << " is late by " << delayMs << "ms. DROPPING!" << std::endl;
        } else {
            // 完全同步 (In Sync)
            std::cout << "[LockFree A/V Sync] Video PTS " << currentFrame.pts
                      << " is ON TIME. Rendering." << std::endl;
            renderFrame(currentFrame);
        }
    }
}

void RenderThreadSyncLoop::renderFrame(const VideoFrame& frame) {
    // 调用 RHI 渲染引擎进行上屏
}

} // namespace media
} // namespace video_sdk
