#pragma once
#include <cstdint>
#include <string>

#ifdef ANDROID
#include <media/NdkMediaCodec.h>
#include <media/NdkMediaFormat.h>
#include <media/NdkMediaMuxer.h>
#else
struct AMediaCodec;
struct AMediaFormat;
struct AMediaMuxer;
#endif

namespace video_sdk {
namespace hal {

/**
 * @brief Android 平台的硬件编码器与混合器封装。
 * 使用 NDK AMediaCodec 和 AMediaMuxer，实现将渲染画面直接编码并写入 MP4。
 */
class AndroidMediaCodecEncoder {
public:
    AndroidMediaCodecEncoder();
    ~AndroidMediaCodecEncoder();

    /**
     * 初始化硬件编码器和 Muxer
     * @param codecId 编码格式 (例如，H.264 或 H.265)
     * @param width 输出视频宽
     * @param height 输出视频高
     * @param bitrate 码率
     * @param fps 帧率
     * @param outputPath MP4 保存路径
     * @return 是否初始化成功
     */
    bool initialize(uint32_t codecId, int width, int height, int bitrate, int fps, const std::string& outputPath);

    /**
     * 获取编码器的 Input Surface
     * 渲染引擎可以直接将画面绘制到此 Surface 上，无需 CPU 介入。
     */
    void* getInputSurface() const { return m_inputSurface; }

    /**
     * 提取编码后的数据包并写入 MP4
     * 在每一帧渲染完后调用此方法排空输出缓冲。
     * @param endOfStream 是否为最后一帧，触发结束标志
     */
    void drainOutput(bool endOfStream = false);

    void destroy();

private:
    bool m_isInitialized = false;
    AMediaCodec* m_codec = nullptr;
    AMediaFormat* m_format = nullptr;
    void* m_inputSurface = nullptr;

    AMediaMuxer* m_muxer = nullptr;
    ssize_t m_videoTrackIndex = -1;
    bool m_muxerStarted = false;
    int m_fd = -1;
};

} // namespace hal
} // namespace video_sdk
