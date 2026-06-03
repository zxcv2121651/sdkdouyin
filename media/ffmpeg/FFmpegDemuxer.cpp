#include "FFmpegDemuxer.h"
#include <iostream>

extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
}

namespace video_sdk {
namespace media {

FFmpegDemuxer::FFmpegDemuxer() {
    m_pkt = av_packet_alloc();
}

FFmpegDemuxer::~FFmpegDemuxer() {
    close();
    if (m_pkt) {
        av_packet_free(&m_pkt);
    }
}

bool FFmpegDemuxer::open(const std::string& filePath) {
    m_filePath = filePath;

    // 1. 打开文件并分配 FormatContext
    if (avformat_open_input(&m_formatCtx, m_filePath.c_str(), nullptr, nullptr) != 0) {
        std::cerr << "Failed to open input: " << m_filePath << std::endl;
        return false;
    }

    // 2. 探测流信息
    if (avformat_find_stream_info(m_formatCtx, nullptr) < 0) {
        std::cerr << "Failed to find stream info." << std::endl;
        close();
        return false;
    }

    // 3. 遍历查找视频流和音频流
    m_videoStreamIndex = -1;
    m_audioStreamIndex = -1;

    for (unsigned int i = 0; i < m_formatCtx->nb_streams; i++) {
        AVStream* stream = m_formatCtx->streams[i];
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            if (m_videoStreamIndex == -1) m_videoStreamIndex = i; // 获取第一个视频流
        } else if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (m_audioStreamIndex == -1) m_audioStreamIndex = i; // 获取第一个音频流
        }
    }

    return (m_videoStreamIndex != -1 || m_audioStreamIndex != -1);
}

void FFmpegDemuxer::close() {
    if (m_formatCtx) {
        avformat_close_input(&m_formatCtx); // 内部会自动将指针置空
    }
    m_videoStreamIndex = -1;
    m_audioStreamIndex = -1;
}

bool FFmpegDemuxer::readPacket(MediaPacket& outPacket) {
    if (!m_formatCtx || !m_pkt) return false;

    // 每次读取前解除上次包的引用，防止内存泄漏
    av_packet_unref(m_pkt);

    int ret = av_read_frame(m_formatCtx, m_pkt);
    if (ret < 0) {
        if (ret == AVERROR_EOF) {
            // EOF 正常结束
        } else {
            // 其他读取错误
        }
        return false;
    }

    // 将 AVPacket 数据映射到我们自己的 MediaPacket
    outPacket.data = m_pkt->data;
    outPacket.size = m_pkt->size;
    outPacket.pts = m_pkt->pts;
    outPacket.dts = m_pkt->dts;
    outPacket.streamIndex = m_pkt->stream_index;
    outPacket.isKeyFrame = (m_pkt->flags & AV_PKT_FLAG_KEY) != 0;

    return true;
}

void FFmpegDemuxer::seekTo(int64_t timestampMs) {
    if (!m_formatCtx) return;

    // 转换时间戳 (默认以 AV_TIME_BASE 为单位)
    // 实际工业级做法需要根据目标 Stream 的 time_base 进行转换
    int streamIndex = (m_videoStreamIndex >= 0) ? m_videoStreamIndex : -1;

    // 往回找最近的关键帧
    av_seek_frame(m_formatCtx, streamIndex, timestampMs, AVSEEK_FLAG_BACKWARD);
}

} // namespace media
} // namespace video_sdk
