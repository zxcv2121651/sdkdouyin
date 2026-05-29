#include "AndroidMediaCodecEncoder.h"
#include <iostream>

#ifdef ANDROID
#include <android/native_window.h>
#endif

namespace video_sdk {
namespace hal {

AndroidMediaCodecEncoder::AndroidMediaCodecEncoder() {}

AndroidMediaCodecEncoder::~AndroidMediaCodecEncoder() {
    destroy();
}

bool AndroidMediaCodecEncoder::initialize(uint32_t codecId, int width, int height, int bitrate, int fps) {
    if (m_isInitialized) return true;

#ifdef ANDROID
    const char* mime = nullptr;
    if (codecId == 27) {
        mime = "video/avc"; // H.264
    } else if (codecId == 173) {
        mime = "video/hevc"; // H.265
    } else {
        std::cerr << "[MediaCodec Encoder] Unsupported codec ID: " << codecId << std::endl;
        return false;
    }

    std::cout << "[MediaCodec Encoder] Initializing encoder for " << mime << std::endl;

    m_codec = AMediaCodec_createEncoderByType(mime);
    if (!m_codec) {
        std::cerr << "[MediaCodec Encoder] Failed to create encoder" << std::endl;
        return false;
    }

    m_format = AMediaFormat_new();
    AMediaFormat_setString(m_format, AMEDIAFORMAT_KEY_MIME, mime);
    AMediaFormat_setInt32(m_format, AMEDIAFORMAT_KEY_WIDTH, width);
    AMediaFormat_setInt32(m_format, AMEDIAFORMAT_KEY_HEIGHT, height);
    AMediaFormat_setInt32(m_format, AMEDIAFORMAT_KEY_BIT_RATE, bitrate);
    AMediaFormat_setInt32(m_format, AMEDIAFORMAT_KEY_FRAME_RATE, fps);
    AMediaFormat_setInt32(m_format, AMEDIAFORMAT_KEY_I_FRAME_INTERVAL, 1);

    // 关键配置：指定颜色格式为 SURFACE，使得编码器能够接收来自 GPU/OpenGL 的数据
    AMediaFormat_setInt32(m_format, AMEDIAFORMAT_KEY_COLOR_FORMAT, 2130708361); // COLOR_FormatSurface

    media_status_t status = AMediaCodec_configure(m_codec, m_format, nullptr, nullptr, 1); // 1 = CONFIGURE_FLAG_ENCODE
    if (status != AMEDIA_OK) {
        std::cerr << "[MediaCodec Encoder] Failed to configure codec" << std::endl;
        return false;
    }

    // 创建 InputSurface (Zero-Copy 的核心)
    status = AMediaCodec_createInputSurface(m_codec, (ANativeWindow**)&m_inputSurface);
    if (status != AMEDIA_OK || !m_inputSurface) {
        std::cerr << "[MediaCodec Encoder] Failed to create input surface" << std::endl;
        return false;
    }

    status = AMediaCodec_start(m_codec);
    if (status != AMEDIA_OK) {
        std::cerr << "[MediaCodec Encoder] Failed to start codec" << std::endl;
        return false;
    }

    m_isInitialized = true;
    return true;
#else
    std::cout << "[MediaCodec Encoder Mock] Initialized hardware encoder on mock platform." << std::endl;
    m_isInitialized = true;
    return true;
#endif
}

void AndroidMediaCodecEncoder::drainOutput() {
    if (!m_isInitialized) return;

#ifdef ANDROID
    AMediaCodecBufferInfo info;
    while (true) {
        ssize_t status = AMediaCodec_dequeueOutputBuffer(m_codec, &info, 0); // Non-blocking
        if (status == AMEDIACODEC_INFO_TRY_AGAIN_LATER) {
            break; // 没有数据可读了
        } else if (status >= 0) {
            size_t bufSize;
            uint8_t* buf = AMediaCodec_getOutputBuffer(m_codec, status, &bufSize);
            if (buf && info.size > 0) {
                // 工业级：在这里将 buf 写入到 MP4 Muxer 中 (如 FFmpeg av_interleaved_write_frame 或 AMediaMuxer)
                // std::cout << "Encoded frame size: " << info.size << std::endl;
            }
            AMediaCodec_releaseOutputBuffer(m_codec, status, false); // false = 不需要 render
        }
    }
#endif
}

void AndroidMediaCodecEncoder::destroy() {
#ifdef ANDROID
    if (m_codec) {
        AMediaCodec_stop(m_codec);
        AMediaCodec_delete(m_codec);
        m_codec = nullptr;
    }
    if (m_format) {
        AMediaFormat_delete(m_format);
        m_format = nullptr;
    }
    // m_inputSurface 的生命周期由系统或特定的 API 管理，如果在 Java 层创建则回传 release
    m_inputSurface = nullptr;
#endif
    m_isInitialized = false;
}

} // namespace hal
} // namespace video_sdk
