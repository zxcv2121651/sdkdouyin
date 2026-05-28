#include "AndroidMediaCodecEncoder.h"
#include <iostream>

namespace video_sdk {
namespace hal {

AndroidMediaCodecEncoder::AndroidMediaCodecEncoder() {}

AndroidMediaCodecEncoder::~AndroidMediaCodecEncoder() {
    stop();
}

bool AndroidMediaCodecEncoder::initialize(const std::string& outputPath, int width, int height, int fps, int bitrate) {
    m_outputPath = outputPath;
    m_width = width;
    m_height = height;

    std::cout << "[AndroidMediaCodecEncoder] Initializing encoder. Output: " << outputPath
              << ", " << width << "x" << height << "@" << fps << "fps, " << bitrate << "bps" << std::endl;

#ifdef ANDROID
    // 实际实现中这里会调用 AMediaCodec_createEncoderByType 和 AMediaFormat_new
    // 配置格式："video/avc" 或 "video/hevc"
    // 设置 width, height, color-format, bit-rate, frame-rate, i-frame-interval
    // m_codec = AMediaCodec_createEncoderByType("video/avc");
    // ... config ...
    // ... create muxer ...
#endif
    return true;
}

void AndroidMediaCodecEncoder::start() {
    if (m_isEncoding) return;
    std::cout << "[AndroidMediaCodecEncoder] Starting encoder." << std::endl;
#ifdef ANDROID
    // AMediaCodec_start(m_codec);
#endif
    m_isEncoding = true;
}

void AndroidMediaCodecEncoder::stop() {
    if (!m_isEncoding) return;
    std::cout << "[AndroidMediaCodecEncoder] Stopping encoder." << std::endl;

    drainEncoder(true);

#ifdef ANDROID
    // AMediaCodec_stop(m_codec);
    // AMediaCodec_delete(m_codec);
    // if (m_muxerStarted) AMediaMuxer_stop(m_muxer);
    // AMediaMuxer_delete(m_muxer);
#endif
    m_isEncoding = false;
}

void AndroidMediaCodecEncoder::encodeFrame(const uint8_t* data, size_t size, int64_t pts) {
    if (!m_isEncoding) return;
    // std::cout << "[AndroidMediaCodecEncoder] Encoding YUV frame, pts: " << pts << std::endl;

#ifdef ANDROID
    // 将 data 送入 input buffer
    // drainEncoder(false);
#endif
}

void AndroidMediaCodecEncoder::encodeTexture(uint32_t textureId, int64_t pts) {
    if (!m_isEncoding) return;
    // std::cout << "[AndroidMediaCodecEncoder] Encoding Texture " << textureId << ", pts: " << pts << std::endl;

#ifdef ANDROID
    // 在基于 Surface 的 MediaCodec 编码中，通常是在 OpenGL 线程里：
    // eglMakeCurrent(encoderSurface)
    // draw textureId
    // eglSwapBuffers(encoderSurface)
    // drainEncoder(false);
#endif
}

void AndroidMediaCodecEncoder::drainEncoder(bool endOfStream) {
#ifdef ANDROID
    // while (true) {
    //     获取 output buffer
    //     如果有效：AMediaMuxer_writeSampleData
    //     如果 INFO_OUTPUT_FORMAT_CHANGED：配置 Muxer 并 start
    //     如果 INFO_TRY_AGAIN_LATER：break
    // }
#endif
}

} // namespace hal
} // namespace video_sdk
