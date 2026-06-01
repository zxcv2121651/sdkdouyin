#include "AndroidMediaCodecEncoder.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

#ifdef ANDROID
#include <android/native_window.h>
#endif

namespace video_sdk {
namespace hal {

AndroidMediaCodecEncoder::AndroidMediaCodecEncoder() {}

AndroidMediaCodecEncoder::~AndroidMediaCodecEncoder() {
    destroy();
}

bool AndroidMediaCodecEncoder::initialize(uint32_t codecId, int width, int height, int bitrate, int fps, const std::string& outputPath) {
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

    std::cout << "[MediaCodec Encoder] Initializing encoder for " << mime << " to " << outputPath << std::endl;

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

    // 创建 InputSurface
    status = AMediaCodec_createInputSurface(m_codec, (ANativeWindow**)&m_inputSurface);
    if (status != AMEDIA_OK || !m_inputSurface) {
        std::cerr << "[MediaCodec Encoder] Failed to create input surface" << std::endl;
        return false;
    }

    // 初始化 Muxer
    m_fd = open(outputPath.c_str(), O_CREAT | O_LARGEFILE | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    if (m_fd < 0) {
        std::cerr << "[MediaCodec Encoder] Failed to open output file: " << outputPath << std::endl;
        return false;
    }

    m_muxer = AMediaMuxer_new(m_fd, AMEDIAMUXER_OUTPUT_FORMAT_MPEG_4);
    if (!m_muxer) {
        std::cerr << "[MediaCodec Encoder] Failed to create muxer" << std::endl;
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
    std::cout << "[MediaCodec Encoder Mock] Initialized hardware encoder and muxer on mock platform. Path: " << outputPath << std::endl;
    m_isInitialized = true;
    return true;
#endif
}

void AndroidMediaCodecEncoder::drainOutput(bool endOfStream) {
    if (!m_isInitialized) return;

#ifdef ANDROID
    if (endOfStream) {
        // 在实际应用中如果能向 InputSurface 发出 EOS 信号最好
        // eglPresentationTimeANDROID 可以用来发时间戳
    }

    AMediaCodecBufferInfo info;
    while (true) {
        ssize_t status = AMediaCodec_dequeueOutputBuffer(m_codec, &info, 0); // Non-blocking
        if (status == AMEDIACODEC_INFO_TRY_AGAIN_LATER) {
            break; // 没有数据可读了
        } else if (status == AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED) {
            if (m_muxerStarted) {
                std::cerr << "[MediaCodec Encoder] format changed twice" << std::endl;
            } else {
                AMediaFormat* newFormat = AMediaCodec_getOutputFormat(m_codec);
                m_videoTrackIndex = AMediaMuxer_addTrack(m_muxer, newFormat);
                AMediaMuxer_start(m_muxer);
                m_muxerStarted = true;
                AMediaFormat_delete(newFormat);
                std::cout << "[MediaCodec Encoder] Muxer started" << std::endl;
            }
        } else if (status >= 0) {
            size_t bufSize;
            uint8_t* buf = AMediaCodec_getOutputBuffer(m_codec, status, &bufSize);

            if ((info.flags & 2) != 0) { // BUFFER_FLAG_CODEC_CONFIG = 2
                info.size = 0;
            }

            if (buf && info.size > 0 && m_muxerStarted) {
                // 将编码好的 H264/H265 NALU 写入 MP4 容器
                AMediaMuxer_writeSampleData(m_muxer, m_videoTrackIndex, buf, &info);
            }

            AMediaCodec_releaseOutputBuffer(m_codec, status, false);

            if ((info.flags & 4) != 0) { // BUFFER_FLAG_END_OF_STREAM = 4
                break;
            }
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
    if (m_muxer) {
        if (m_muxerStarted) {
            AMediaMuxer_stop(m_muxer);
        }
        AMediaMuxer_delete(m_muxer);
        m_muxer = nullptr;
    }
    if (m_fd >= 0) {
        close(m_fd);
        m_fd = -1;
    }
    if (m_format) {
        AMediaFormat_delete(m_format);
        m_format = nullptr;
    }
    m_inputSurface = nullptr;
    m_muxerStarted = false;
#endif
    m_isInitialized = false;
}

} // namespace hal
} // namespace video_sdk
