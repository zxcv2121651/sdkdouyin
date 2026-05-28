#pragma once
#include "hal/interface/IMediaEncoder.h"
#include <string>

#ifdef ANDROID
#include <media/NdkMediaCodec.h>
#include <media/NdkMediaFormat.h>
#include <media/NdkMediaMuxer.h>
#endif

namespace video_sdk {
namespace hal {

/**
 * @brief Android 平台基于 MediaCodec 和 MediaMuxer 的硬件编码器实现。
 */
class AndroidMediaCodecEncoder : public IMediaEncoder {
public:
    AndroidMediaCodecEncoder();
    ~AndroidMediaCodecEncoder() override;

    bool initialize(const std::string& outputPath, int width, int height, int fps, int bitrate) override;
    void start() override;
    void stop() override;
    void encodeFrame(const uint8_t* data, size_t size, int64_t pts) override;
    void encodeTexture(uint32_t textureId, int64_t pts) override;

private:
    void drainEncoder(bool endOfStream);

private:
    int m_width = 0;
    int m_height = 0;
    std::string m_outputPath;
    bool m_isEncoding = false;

#ifdef ANDROID
    AMediaCodec* m_codec = nullptr;
    AMediaMuxer* m_muxer = nullptr;
    ssize_t m_trackIndex = -1;
    bool m_muxerStarted = false;
#else
    void* m_codec = nullptr; // Mock
    void* m_muxer = nullptr; // Mock
#endif
};

} // namespace hal
} // namespace video_sdk
