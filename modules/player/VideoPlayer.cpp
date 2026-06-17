#include "VideoPlayer.h"
#include <iostream>

namespace video_sdk {
namespace modules {

VideoPlayer::VideoPlayer() {
    m_renderThread = std::make_shared<core::RenderThread>();
    m_renderThread->startRenderThread();
    std::cout << "[VideoPlayer] Initialized with unified RenderThread." << std::endl;
}

VideoPlayer::~VideoPlayer() {
    if (m_renderThread) {
        m_renderThread->stopRenderThread();
    }
}

void VideoPlayer::setWindow(void* window) {
    // 将绑定窗口的操作异步丢给渲染线程，因为涉及到 EGLSurface 的创建
    m_renderThread->postTask([this, window]() {
        m_window = window;
        std::cout << "[VideoPlayer] Bound window " << window << " on RenderThread." << std::endl;
        // 实际上这里应该调用 m_renderer->setWindow(window) 等操作
    });
}

bool VideoPlayer::prepare(const std::string& sourceUrl) {
    m_renderThread->postTask([this, sourceUrl]() {
        std::cout << "[VideoPlayer] Preparing " << sourceUrl << " on RenderThread." << std::endl;
    });
    return true; // Mock true for synchronous return
}

void VideoPlayer::play() {
    m_isPlaying = true;
    m_renderThread->postTask([this]() {
        std::cout << "[VideoPlayer] Started playing on RenderThread." << std::endl;
        // 唤醒底层的 RenderThreadSyncLoop (A/V Sync)
    });
}

void VideoPlayer::pause() {
    m_isPlaying = false;
    m_renderThread->postTask([this]() {
        std::cout << "[VideoPlayer] Paused on RenderThread." << std::endl;
        // 挂起 A/V Sync
    });
}

void VideoPlayer::seekTo(int64_t timestampMs) {
    m_currentPositionMs = timestampMs;
    // 使用 postTaskAndReply 可以让 UI 层阻塞等待 seek 完成，避免花屏
    m_renderThread->postTaskAndReply([this, timestampMs]() {
        std::cout << "[VideoPlayer] Seeked to " << timestampMs << "ms on RenderThread." << std::endl;
        // 执行底层的 seek 逻辑，Flush Decoder 等
    }).wait();
}

int64_t VideoPlayer::getCurrentPosition() const {
    return m_currentPositionMs;
}

} // namespace modules
} // namespace video_sdk
