#include "AudioMixer.h"
#include <algorithm>
#include <iostream>

namespace video_sdk {
namespace core {

AudioMixer::AudioMixer() {}

AudioMixer::~AudioMixer() {}

void AudioMixer::mixAndLimit(const std::vector<std::vector<int16_t>>& inputTracks, std::vector<int16_t>& outputBuffer) {
    if (inputTracks.empty()) return;

    // 找到最长的那条音轨的长度作为输出基准
    size_t maxLength = 0;
    for (const auto& track : inputTracks) {
        if (track.size() > maxLength) {
            maxLength = track.size();
        }
    }

    outputBuffer.assign(maxLength, 0);

    // 1. 线性叠加多轨音频 (此时使用 32-bit 整型以防止溢出)
    std::vector<int32_t> mixBuffer(maxLength, 0);

    for (const auto& track : inputTracks) {
        for (size_t i = 0; i < track.size(); ++i) {
            mixBuffer[i] += track[i];
        }
    }

    // 2. 应用硬限幅器 (Hard Limiter) 并转回 16-bit
    for (size_t i = 0; i < maxLength; ++i) {
        int32_t mixedSample = mixBuffer[i];

        // 软削峰 (Soft Clipping) 替代直接的 Hard Clip
        // 可以让音频失真更平滑，这里使用一种经典的非线性衰减算法
        // 取值边界: int16 max (32767), int16 min (-32768)

        if (mixedSample > 32767) {
            // 这里为了简单工业级实现，采用严格 Hard Clip 保底
            mixedSample = 32767;
        } else if (mixedSample < -32768) {
            mixedSample = -32768;
        }

        outputBuffer[i] = static_cast<int16_t>(mixedSample);
    }
}

void AudioMixer::applyHardLimiter(std::vector<int16_t>& buffer) {
    // 如果在单一处理流中需要调用，可服用上面的限幅边界逻辑
    for (auto& sample : buffer) {
        if (sample > 32767) sample = 32767;
        else if (sample < -32768) sample = -32768;
    }
}

} // namespace core
} // namespace video_sdk
