#pragma once
#include <string>
#include <vector>
#include <cstdint>

// 前置声明 FFmpeg 核心结构
struct AVFormatContext;
struct AVPacket;

namespace video_sdk {
namespace media {

// 框架内部封装的 Packet 结构，隔离上层代码对 FFmpeg 宏的直接依赖
struct MediaPacket {
    uint8_t* data = nullptr; // 压缩后的流数据指针 (例如 H.264 的 NALU)
    int size = 0;            // 数据大小
    int64_t pts = 0;         // Presentation Time Stamp (显示时间戳)
    int64_t dts = 0;         // Decoding Time Stamp (解码时间戳)
    int streamIndex = -1;    // 轨道索引：属于视频轨还是音频轨
    bool isKeyFrame = false; // 是否为 I 帧 (关键帧)，用于 Seek 控制
};

/**
 * @brief FFmpeg 解封装器 (Demuxer) 核心实现。
 *
 * 核心原理：
 * 在音视频处理中，MP4、MKV 等只是"容器(Container)"。里面同时装了视频流、音频流和字幕流。
 * Demuxer 的任务就是打开这个容器（利用 libavformat），把它剥开，把里面压缩的视频数据 (AVPacket)
 * 和音频数据分别提取出来，送给下方的解码器 (Decoder) 去解压缩。
 */
class FFmpegDemuxer {
public:
    FFmpegDemuxer();
    ~FFmpegDemuxer();

    /**
     * @brief 打开视频文件并解析流信息 (Probe Stream Info)。
     * 内部会调用 avformat_open_input 和 avformat_find_stream_info，
     * 找出里面哪条轨道是视频，哪条是音频。
     */
    bool open(const std::string& filePath);

    /**
     * 关闭文件并释放解封装上下文资源。
     */
    void close();

    /**
     * @brief 顺序读取容器中的下一个压缩数据包。
     * 这是驱动整个播放和解码流水线的源头。
     * @param outPacket [输出] 提取到的音/视频包
     * @return 成功返回 true，读到文件末尾 (EOF) 返回 false
     */
    bool readPacket(MediaPacket& outPacket);

    // 获取视频流和音频流在容器中的索引号，用于过滤不需要的数据
    int getVideoStreamIndex() const { return m_videoStreamIndex; }
    int getAudioStreamIndex() const { return m_audioStreamIndex; }

    /**
     * @brief 精准跳转 (Seek) 到指定时间。
     * 工业级实现：底层会调用 av_seek_frame，并且通常需要 Seek 到离目标时间最近的
     * 一个关键帧 (I帧 / AVSEEK_FLAG_BACKWARD)，然后让解码器快速解码但不渲染，直到准确命中目标时间。
     */
    void seekTo(int64_t timestampMs);

    // 暴露内部的上下文给外部 Decoder 获取解码所需的 extradata (如 SPS/PPS)
    AVFormatContext* getFormatContext() const { return m_formatCtx; }

private:
    std::string m_filePath;
    int m_videoStreamIndex = -1;
    int m_audioStreamIndex = -1;

    AVFormatContext* m_formatCtx = nullptr;
    AVPacket* m_pkt = nullptr;
};

} // namespace media
} // namespace video_sdk
