#pragma once
#include <vector>
#include <cstdint>

namespace video_sdk {
namespace core {

/**
 * @brief AudioMixer handles multi-track audio mixing.
 * Integrates a hard limiter to prevent audio clipping.
 */
class AudioMixer {
public:
    AudioMixer();
    ~AudioMixer();

    // Mix multiple PCM streams and apply hard limiter
    void mixAndLimit(const std::vector<std::vector<int16_t>>& inputTracks, std::vector<int16_t>& outputBuffer);

private:
    void applyHardLimiter(std::vector<int16_t>& buffer);
};

} // namespace core
} // namespace video_sdk
