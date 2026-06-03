#include "AndroidMediaCodecDecoder.h"
#include <iostream>

#ifdef ANDROID
#include <android/native_window.h>
#endif

namespace video_sdk {
namespace hal {

AndroidMediaCodecDecoder::AndroidMediaCodecDecoder() {}

AndroidMediaCodecDecoder::~AndroidMediaCodecDecoder() {
    destroy();
}

bool AndroidMediaCodecDecoder::initialize(uint32_t codecId, int width, int height, void* nativeWindow) {
    if (m_isInitialized) return true;

#ifdef ANDROID
    const char* mime = nullptr;
    // 假设 27 是 H264，173 是 H265 (HEVC) - 基于 FFmpeg 的 codec_id 约定
    if (codecId == 27) {
        mime = "video/avc"; // H.264
    } else if (codecId == 173) {
        mime = "video/hevc"; // H.265
    } else {
        std::cerr << "[MediaCodec] Unsupported codec ID: " << codecId << std::endl;
        return false;
    }

    std::cout << "[MediaCodec] Initializing decoder for " << mime << std::endl;

    m_codec = AMediaCodec_createDecoderByType(mime);
    if (!m_codec) {
        std::cerr << "[MediaCodec] Failed to create decoder" << std::endl;
        return false;
    }

    m_format = AMediaFormat_new();
    AMediaFormat_setString(m_format, AMEDIAFORMAT_KEY_MIME, mime);
    AMediaFormat_setInt32(m_format, AMEDIAFORMAT_KEY_WIDTH, width);
    AMediaFormat_setInt32(m_format, AMEDIAFORMAT_KEY_HEIGHT, height);

    // nativeWindow 通常是从 Java 层的 Surface 或 SurfaceTexture 获取的 ANativeWindow
    media_status_t status = AMediaCodec_configure(m_codec, m_format, static_cast<ANativeWindow*>(nativeWindow), nullptr, 0);
    if (status != AMEDIA_OK) {
        std::cerr << "[MediaCodec] Failed to configure codec" << std::endl;
        return false;
    }

    status = AMediaCodec_start(m_codec);
    if (status != AMEDIA_OK) {
        std::cerr << "[MediaCodec] Failed to start codec" << std::endl;
        return false;
    }

    m_isInitialized = true;
    return true;
#else
    std::cout << "[MediaCodec Mock] Initialized hardware decoder on mock platform." << std::endl;
    m_isInitialized = true;
    return true;
#endif
}

void AndroidMediaCodecDecoder::destroy() {
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
#endif
    m_isInitialized = false;
}

bool AndroidMediaCodecDecoder::sendPacket(const uint8_t* data, int size, int64_t pts) {
    if (!m_isInitialized) return false;

#ifdef ANDROID
    ssize_t bufIdx = AMediaCodec_dequeueInputBuffer(m_codec, 2000); // timeoutUs = 2000
    if (bufIdx >= 0) {
        size_t bufSize;
        uint8_t* buf = AMediaCodec_getInputBuffer(m_codec, bufIdx, &bufSize);
        if (buf && size <= bufSize) {
            memcpy(buf, data, size);
            AMediaCodec_queueInputBuffer(m_codec, bufIdx, 0, size, pts, 0);
            return true;
        }
    }
    return false;
#else
    return true; // Mock success
#endif
}

bool AndroidMediaCodecDecoder::receiveFrame() {
    if (!m_isInitialized) return false;

#ifdef ANDROID
    AMediaCodecBufferInfo info;
    ssize_t status = AMediaCodec_dequeueOutputBuffer(m_codec, &info, 2000);

    if (status >= 0) {
        // status 即为 buffer index
        // 关键点：render = true。
        // 这指示 MediaCodec 直接将解码出的画面发送到初始化时绑定的 Surface 上。
        // CPU 不读取任何像素数据，实现了真正的零拷贝 (Zero-Copy)！
        AMediaCodec_releaseOutputBuffer(m_codec, status, true);
        return true;
    } else if (status == AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED) {
        // Output buffers changed
    } else if (status == AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED) {
        // Output format changed
    } else if (status == AMEDIACODEC_INFO_TRY_AGAIN_LATER) {
        // Try again
    }
    return false;
#else
    return true; // Mock success
#endif
}

} // namespace hal
} // namespace video_sdk
