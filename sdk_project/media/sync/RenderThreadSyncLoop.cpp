#include "RenderThreadSyncLoop.h"
#include <iostream>
#include <chrono>

namespace video_sdk {
namespace media {

RenderThreadSyncLoop::RenderThreadSyncLoop(std::shared_ptr<AVSyncClock> masterClock)
    : m_masterClock(masterClock) {
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
    m_queueCondVar.notify_all();
    if (m_renderThread.joinable()) {
        m_renderThread.join();
    }
}

void RenderThreadSyncLoop::enqueueFrame(const VideoFrame& frame) {
    std::lock_guard<std::mutex> lock(m_queueMutex);
    m_frameQueue.push(frame);
    m_queueCondVar.notify_one();
}

void RenderThreadSyncLoop::threadLoop() {
    while (m_isRunning) {
        VideoFrame currentFrame;
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_queueCondVar.wait(lock, [this]() {
                return !m_isRunning || !m_frameQueue.empty();
            });

            if (!m_isRunning && m_frameQueue.empty()) break;

            currentFrame = m_frameQueue.front();
            m_frameQueue.pop();
        }

        // --- 核心音视频同步逻辑 (A/V Sync) ---
        // 1. 获取这帧视频与主时钟（音频）的延迟差值
        int64_t delayMs = m_masterClock->computeVideoDelay(currentFrame.pts);

        if (delayMs > SYNC_THRESHOLD_MAX) {
            // 状态 1: 视频偏早 (Video is early)
            // 解决: 渲染线程需要睡眠等待，直到该帧的时间戳临近主时钟
            std::cout << "[A/V Sync] Video PTS " << currentFrame.pts
                      << " is early by " << delayMs << "ms. Sleeping..." << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
            renderFrame(currentFrame);

        } else if (delayMs < SYNC_THRESHOLD_MIN) {
            // 状态 2: 视频滞后 (Video is late)
            // 解决: 直接丢弃这帧视频，不渲染，快速追赶主时钟 (Drop Frame)
            std::cout << "[A/V Sync] Video PTS " << currentFrame.pts
                      << " is late by " << delayMs << "ms. DROPPING FRAME!" << std::endl;
            // 丢帧，什么也不做

        } else {
            // 状态 3: 完全同步范围内 (In Sync Threshold)
            // 解决: 立即渲染
            std::cout << "[A/V Sync] Video PTS " << currentFrame.pts
                      << " is ON TIME (diff " << delayMs << "ms). Rendering immediately." << std::endl;
            renderFrame(currentFrame);
        }
    }
}

void RenderThreadSyncLoop::renderFrame(const VideoFrame& frame) {
    // 调用 RHI 渲染引擎进行 OES->RGB 或者 YUV->RGB 的转换及上屏
    // FilterEngine->applyFilter(...)
}

} // namespace media
} // namespace video_sdk
