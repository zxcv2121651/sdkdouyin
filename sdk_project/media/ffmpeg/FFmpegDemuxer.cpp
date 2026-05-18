#include "FFmpegDemuxer.h"

namespace video_sdk {
namespace media {

FFmpegDemuxer::FFmpegDemuxer() {}

FFmpegDemuxer::~FFmpegDemuxer() {
    close();
}

bool FFmpegDemuxer::open(const std::string& filePath) {
    m_filePath = filePath;
    // 1. 调用 avformat_open_input 打开文件
    // 2. 调用 avformat_find_stream_info 解析流信息
    // 3. 遍历流寻找 AVMEDIA_TYPE_VIDEO 和 AVMEDIA_TYPE_AUDIO 并记录索引
    return true;
}

void FFmpegDemuxer::close() {
    // 释放 avformat_close_input 等上下文资源
    m_videoStreamIndex = -1;
    m_audioStreamIndex = -1;
}

bool FFmpegDemuxer::readPacket(MediaPacket& outPacket) {
    // 调用 av_read_frame 获取下一帧
    // 这里仅做占位返回 false
    return false;
}

void FFmpegDemuxer::seekTo(int64_t timestampMs) {
    // 使用 avformat_seek_file 进行精确或者关键帧跳转
}

} // namespace media
} // namespace video_sdk
