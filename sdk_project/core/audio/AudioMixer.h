#pragma once
#include <vector>
#include <cstdint>

namespace video_sdk {
namespace core {

/**
 * @brief AudioMixer 负责多轨音频混音处理。
 * 内部集成了硬限幅器（Hard Limiter），用于防止波形失真（爆音）。
 */
class AudioMixer {
public:
    AudioMixer();
    ~AudioMixer();

    // 混合多条 PCM 音频流并应用硬限幅器
    void mixAndLimit(const std::vector<std::vector<int16_t>>& inputTracks, std::vector<int16_t>& outputBuffer);

private:
    // 应用硬限幅器
    void applyHardLimiter(std::vector<int16_t>& buffer);
};

} // namespace core
} // namespace video_sdk
