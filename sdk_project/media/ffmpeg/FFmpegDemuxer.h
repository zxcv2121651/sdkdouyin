#pragma once
#include <string>
#include <vector>
#include <cstdint>

// 前置声明
struct AVFormatContext;
struct AVPacket;

namespace video_sdk {
namespace media {

// 模拟的 Packet 结构，实际使用时映射 AVPacket
struct MediaPacket {
    uint8_t* data = nullptr;
    int size = 0;
    int64_t pts = 0;
    int64_t dts = 0;
    int streamIndex = -1;
    bool isKeyFrame = false;
};

/**
 * @brief FFmpeg 解封装器深度实现。
 * 负责打开音视频文件（使用 libavformat），解析出 Stream 信息，
 * 并将数据解复用为音频和视频的 Packet 队列。
 */
class FFmpegDemuxer {
public:
    FFmpegDemuxer();
    ~FFmpegDemuxer();

    // 打开视频文件并解析流信息
    bool open(const std::string& filePath);

    // 关闭文件并释放资源
    void close();

    // 读取下一个数据包
    bool readPacket(MediaPacket& outPacket);

    // 获取视频流和音频流的索引
    int getVideoStreamIndex() const { return m_videoStreamIndex; }
    int getAudioStreamIndex() const { return m_audioStreamIndex; }

    // 跳转到指定时间 (Seek)
    void seekTo(int64_t timestampMs);

    // 暴露内部的上下文给外部 Decoder 获取参数
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
