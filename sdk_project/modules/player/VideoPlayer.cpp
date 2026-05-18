#include "VideoPlayer.h"

namespace video_sdk {
namespace modules {

VideoPlayer::VideoPlayer() {}

VideoPlayer::~VideoPlayer() {
    pause();
}

bool VideoPlayer::prepare(const std::string& sourceUrl) {
    // 1. 初始化 Demuxer 和 Decoder
    // 2. 初始化 Timeline 和 音视频同步时钟
    return true;
}

void VideoPlayer::play() {
    if (m_isPlaying) return;
    m_isPlaying = true;
    // 唤醒解码与渲染线程
}

void VideoPlayer::pause() {
    if (!m_isPlaying) return;
    m_isPlaying = false;
    // 挂起解码与渲染线程
}

void VideoPlayer::seekTo(int64_t timestampMs) {
    // 通知底层 Demuxer 与 Decoder Flush 缓冲区
    // 重置 AVSyncClock 并刷新渲染管线
    m_currentPositionMs = timestampMs;
}

int64_t VideoPlayer::getCurrentPosition() const {
    return m_currentPositionMs;
}

} // namespace modules
} // namespace video_sdk
