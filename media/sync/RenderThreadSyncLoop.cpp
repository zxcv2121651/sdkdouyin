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

/**
 * @brief 启动渲染消息循环线程。
 * 这个线程将常驻后台，等待解码器塞入视频帧或等待定时器触发。
 */
void RenderThreadSyncLoop::start() {
    m_messageLoop.start("RenderSyncThread");
}

void RenderThreadSyncLoop::stop() {
    m_messageLoop.stop();
}

void RenderThreadSyncLoop::postTask(std::function<void()> task) {
    m_messageLoop.postTask(std::move(task));
}

/**
 * @brief 接收解码后的视频帧 (通常由解码线程调用)。
 * 使用无锁环形队列 (LockFreeRingBuffer) 进行跨线程传递，极速且不阻塞。
 */
bool RenderThreadSyncLoop::enqueueFrame(const VideoFrame& frame) {
    if (!m_frameQueue.push(frame)) {
        return false; // 队列满了，由解码线程决定是否自旋重试
    }

    // 数据进队后，必须唤醒渲染线程立刻做一次检查调度
    m_messageLoop.postTask([this]() {
        this->scheduleNextRender();
    });
    return true;
}

/**
 * @brief 核心音视频同步调度算法 (A/V Sync Scheduling)。
 * 通过对比音轨的主时钟 (Master Clock) 和当前视频帧的 PTS (Presentation Time Stamp)，
 * 决定是丢弃该帧 (Drop)、立即渲染 (Render) 还是挂起等待 (Delay/Sleep)。
 */
void RenderThreadSyncLoop::scheduleNextRender() {
    if (!m_messageLoop.isRunning() || m_frameQueue.empty()) return;

    VideoFrame frame;
    // 取出最老的帧 (只窥探 peek，不直接出队 pop，因为如果太早我们需要挂起等待)
    if (!m_frameQueue.peek(frame)) return;

    int64_t audioClockMs = m_masterClock->getClock(); // 获取绝对标准时间 (音频时钟)
    int64_t diffMs = frame.pts - audioClockMs;

    if (diffMs > SYNC_THRESHOLD_MAX) {
        // 视频帧太早了 (提前量大于10ms)，意味着我们需要等待一会再渲染
        // 【关键优化】：坚决不用 std::this_thread::sleep_for！
        // 而是将调度任务扔给底层的 ConditionVariable 进行零 CPU 占用休眠。
        m_messageLoop.postDelayedTask([this]() {
            this->scheduleNextRender();
        }, diffMs);
    }
    else if (diffMs < SYNC_THRESHOLD_MIN) {
        // 视频帧太晚了 (落后音频超过15ms)，画面已经过时
        std::cout << "[LockFree A/V Sync] Video PTS " << frame.pts << " is too LATE (diff " << diffMs << "ms). DROPPING!" << std::endl;
        m_frameQueue.pop(); // 直接丢弃当前废帧

        // 丢完后立刻调度处理下一帧，防止连环卡顿
        m_messageLoop.postTask([this]() {
            this->scheduleNextRender();
        });
    }
    else {
        // 在 [-15ms, 10ms] 黄金阈值范围内，说明视频帧正好踩在点上，准时送去渲染
        m_frameQueue.pop(); // 真正将该帧出队
        renderFrame(frame);

        // 渲染完本帧后，立刻检查队列里是不是还有下一帧
        m_messageLoop.postTask([this]() {
            this->scheduleNextRender();
        });
    }
}

void RenderThreadSyncLoop::renderFrame(const VideoFrame& frame) {
    // 工业级实现：在这里取出 YUV data 或 TextureID，送入 RenderGraph 进行链式处理和特效绘制
    // m_renderGraph->render(frame);
    // m_eglCore->swapBuffers();
    std::cout << "[LockFree A/V Sync] Video PTS " << frame.pts << " is ON TIME. Rendering." << std::endl;
}

} // namespace media
} // namespace video_sdk
