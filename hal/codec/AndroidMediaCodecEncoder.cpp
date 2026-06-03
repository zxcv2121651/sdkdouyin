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

/**
 * @brief 初始化 Android 硬件编码器 (AMediaCodec) 以及 MP4 封装器 (AMediaMuxer)。
 * 配置为直接接收 Surface 数据（COLOR_FormatSurface），避免 CPU 发生 YUV 像素拷贝。
 */
bool AndroidMediaCodecEncoder::initialize(uint32_t codecId, int width, int height, int bitrate, int fps, const std::string& outputPath) {
    if (m_isInitialized) return true;

#ifdef ANDROID
    const char* mime = nullptr;
    if (codecId == 27) {
        mime = "video/avc"; // 对应 H.264
    } else if (codecId == 173) {
        mime = "video/hevc"; // 对应 H.265 (HEVC)
    } else {
        std::cerr << "[MediaCodec Encoder] Unsupported codec ID: " << codecId << std::endl;
        return false;
    }

    std::cout << "[MediaCodec Encoder] Initializing encoder for " << mime << " to " << outputPath << std::endl;

    m_codec = AMediaCodec_createEncoderByType(mime);
    if (!m_codec) return false;

    // 配置编码参数 (宽、高、码率、帧率、关键帧间隔)
    m_format = AMediaFormat_new();
    AMediaFormat_setString(m_format, AMEDIAFORMAT_KEY_MIME, mime);
    AMediaFormat_setInt32(m_format, AMEDIAFORMAT_KEY_WIDTH, width);
    AMediaFormat_setInt32(m_format, AMEDIAFORMAT_KEY_HEIGHT, height);
    AMediaFormat_setInt32(m_format, AMEDIAFORMAT_KEY_BIT_RATE, bitrate);
    AMediaFormat_setInt32(m_format, AMEDIAFORMAT_KEY_FRAME_RATE, fps);
    AMediaFormat_setInt32(m_format, AMEDIAFORMAT_KEY_I_FRAME_INTERVAL, 1);

    // 【核心黑科技】：指定颜色格式为 SURFACE。
    // 这将使得编码器能够接收一个由 GPU(OpenGL/Vulkan) 直接写入的 InputSurface，彻底免除 CPU 数据中转。
    AMediaFormat_setInt32(m_format, AMEDIAFORMAT_KEY_COLOR_FORMAT, 2130708361); // COLOR_FormatSurface

    media_status_t status = AMediaCodec_configure(m_codec, m_format, nullptr, nullptr, 1); // 1 = CONFIGURE_FLAG_ENCODE
    if (status != AMEDIA_OK) return false;

    // 创建 InputSurface 给上层 RenderGraph 使用
    status = AMediaCodec_createInputSurface(m_codec, (ANativeWindow**)&m_inputSurface);
    if (status != AMEDIA_OK || !m_inputSurface) return false;

    // 初始化 MP4 Muxer 文件封装器
    m_fd = open(outputPath.c_str(), O_CREAT | O_LARGEFILE | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    if (m_fd < 0) return false;

    m_muxer = AMediaMuxer_new(m_fd, AMEDIAMUXER_OUTPUT_FORMAT_MPEG_4);
    if (!m_muxer) return false;

    // 启动编码芯片硬件逻辑
    status = AMediaCodec_start(m_codec);
    if (status != AMEDIA_OK) return false;

    m_isInitialized = true;
    return true;
#else
    std::cout << "[MediaCodec Encoder Mock] Initialized hardware encoder and muxer on mock platform. Path: " << outputPath << std::endl;
    m_isInitialized = true;
    return true;
#endif
}

/**
 * @brief 排空硬件编码器的输出缓冲。
 * 每一帧画面被 GPU 画到 InputSurface 后，硬件芯片会在后台异步压制成 H.264/H.265 的 NALU 数据包。
 * 业务层需要不断调用这个方法，把压好的裸流数据包捞出来，塞进 MP4 Muxer 中写入文件。
 */
void AndroidMediaCodecEncoder::drainOutput(bool endOfStream) {
    if (!m_isInitialized) return;

#ifdef ANDROID
    if (endOfStream) {
        // 在工业级实现中，此处通常调用 AMediaCodec_signalEndOfInputStream()
        // 告知底层硬件：不再有新的画面进入，请把缓存里的尾部数据全部压完吐出来。
    }

    AMediaCodecBufferInfo info;
    while (true) {
        // 使用非阻塞模式 (timeout=0) 取出编码好的数据
        ssize_t status = AMediaCodec_dequeueOutputBuffer(m_codec, &info, 0);

        if (status == AMEDIACODEC_INFO_TRY_AGAIN_LATER) {
            break; // 硬件还在拼命压制中，目前没有新的包可以取，跳出循环
        } else if (status == AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED) {
            // 编码器正式确定了输出格式 (SPS/PPS 准备好了)，此时可以启动 Muxer 写入 MP4 头部轨道信息
            if (!m_muxerStarted) {
                AMediaFormat* newFormat = AMediaCodec_getOutputFormat(m_codec);
                m_videoTrackIndex = AMediaMuxer_addTrack(m_muxer, newFormat);
                AMediaMuxer_start(m_muxer);
                m_muxerStarted = true;
                AMediaFormat_delete(newFormat);
                std::cout << "[MediaCodec Encoder] Muxer started" << std::endl;
            }
        } else if (status >= 0) {
            // 成功取到一个有效的数据包 (buf)
            size_t bufSize;
            uint8_t* buf = AMediaCodec_getOutputBuffer(m_codec, status, &bufSize);

            if ((info.flags & 2) != 0) { // BUFFER_FLAG_CODEC_CONFIG = 2 (头信息)
                info.size = 0;
            }

            if (buf && info.size > 0 && m_muxerStarted) {
                // 将编码好的 H264/H265 NALU 真实写入 Android 文件系统中的 MP4 容器
                AMediaMuxer_writeSampleData(m_muxer, m_videoTrackIndex, buf, &info);
            }

            // 无论写没写，都必须释放 Buffer，否则硬件芯片会因为可用输出缓冲耗尽而卡死！
            AMediaCodec_releaseOutputBuffer(m_codec, status, false);

            if ((info.flags & 4) != 0) { // BUFFER_FLAG_END_OF_STREAM = 4
                break; // 整个视频流彻底结束
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
