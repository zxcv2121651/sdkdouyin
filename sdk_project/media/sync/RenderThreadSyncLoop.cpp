#include "RenderThreadSyncLoop.h"
#include <iostream>

namespace video_sdk {
namespace media {

RenderThreadSyncLoop::RenderThreadSyncLoop(std::shared_ptr<AVSyncClock> masterClock, size_t bufferSize)
    : m_masterClock(masterClock), m_frameQueue(bufferSize) {
}

RenderThreadSyncLoop::~RenderThreadSyncLoop() {
    stop();
}

void RenderThreadSyncLoop::start() {
    m_messageLoop.start("RenderSyncThread");
}

void RenderThreadSyncLoop::stop() {
    m_messageLoop.stop();
}

void RenderThreadSyncLoop::postTask(std::function<void()> task) {
    m_messageLoop.postTask(std::move(task));
}

bool RenderThreadSyncLoop::enqueueFrame(const VideoFrame& frame) {
    if (!m_frameQueue.push(frame)) {
        return false;
    }

    // 数据进入队列后，抛一个任务去触发调度
    m_messageLoop.postTask([this]() {
        this->scheduleNextRender();
    });
    return true;
}

void RenderThreadSyncLoop::scheduleNextRender() {
    if (!m_messageLoop.isRunning() || m_frameQueue.empty()) return;

    VideoFrame frame;
    // 取出最老的帧 (但不从队列中删除，直到我们决定渲染或丢弃它)
    if (!m_frameQueue.peek(frame)) return;

    int64_t audioClockMs = m_masterClock->getClock();
    int64_t diffMs = frame.pts - audioClockMs;

    if (diffMs > SYNC_THRESHOLD_MAX) {
        // 视频帧太早了 (提前量大于10ms)，使用 postDelayedTask 精准延后调度
        // 取消了原来的 std::this_thread::sleep_for，释放 CPU
        m_messageLoop.postDelayedTask([this]() {
            this->scheduleNextRender();
        }, diffMs);
    }
    else if (diffMs < SYNC_THRESHOLD_MIN) {
        // 视频帧太晚了 (落后超过15ms)，丢帧并立即调度下一帧
        std::cout << "[LockFree A/V Sync] Video PTS " << frame.pts << " is too LATE (diff " << diffMs << "ms). DROPPING!" << std::endl;
        m_frameQueue.pop(); // 丢弃当前帧

        m_messageLoop.postTask([this]() {
            this->scheduleNextRender();
        });
    }
    else {
        // 在阈值范围内，准时渲染
        m_frameQueue.pop(); // 真正取出
        renderFrame(frame);

        // 渲染完后立刻检查是否还有下一帧
        m_messageLoop.postTask([this]() {
            this->scheduleNextRender();
        });
    }
}

void RenderThreadSyncLoop::renderFrame(const VideoFrame& frame) {
    // 工业级实现：在这里取出 YUV data 或 TextureID，送入 RenderGraph 进行链式处理和特效绘制
    // RenderGraph->render(frame);
    std::cout << "[LockFree A/V Sync] Video PTS " << frame.pts << " is ON TIME. Rendering." << std::endl;
}

} // namespace media
} // namespace video_sdk
