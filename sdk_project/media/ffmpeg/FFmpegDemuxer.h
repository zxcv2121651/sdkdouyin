#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace video_sdk {
namespace media {

// 模拟的 Packet 结构，实际使用时对应 AVPacket
struct MediaPacket {
    uint8_t* data;
    int size;
    int64_t pts;
    int64_t dts;
    int streamIndex;
    bool isKeyFrame;
};

/**
 * @brief FFmpeg 解封装器。
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

private:
    std::string m_filePath;
    int m_videoStreamIndex = -1;
    int m_audioStreamIndex = -1;

    // 占位：实际开发时需要包含 AVFormatContext* 等 FFmpeg 结构体
};

} // namespace media
} // namespace video_sdk
