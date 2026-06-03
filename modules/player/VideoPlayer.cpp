#include "VideoPlayer.h"
#include <iostream>

namespace video_sdk {
namespace modules {

VideoPlayer::VideoPlayer() {}
VideoPlayer::~VideoPlayer() {}

void VideoPlayer::setWindow(void* window) {
    m_window = window;
    std::cout << "[VideoPlayer] Bound window surface: " << m_window << std::endl;
}

bool VideoPlayer::prepare(const std::string& sourceUrl) {
    std::cout << "[VideoPlayer] Prepared: " << sourceUrl << std::endl;
    return true;
}

void VideoPlayer::play() {
    m_isPlaying = true;
    std::cout << "[VideoPlayer] Play." << std::endl;
}

void VideoPlayer::pause() {
    m_isPlaying = false;
    std::cout << "[VideoPlayer] Pause." << std::endl;
}

void VideoPlayer::seekTo(int64_t timestampMs) {
    m_currentPositionMs = timestampMs;
    std::cout << "[VideoPlayer] Seek to " << timestampMs << " ms." << std::endl;
}

int64_t VideoPlayer::getCurrentPosition() const {
    return m_currentPositionMs;
}

} // namespace modules
} // namespace video_sdk
