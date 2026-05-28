#include "SoftwareVideoDecoder.h"
#include <iostream>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
}

namespace video_sdk {
namespace media {

SoftwareVideoDecoder::SoftwareVideoDecoder() {
    m_frame = av_frame_alloc();
    m_pkt = av_packet_alloc();
}

SoftwareVideoDecoder::~SoftwareVideoDecoder() {
    destroy();
    if (m_frame) av_frame_free(&m_frame);
    if (m_pkt) av_packet_free(&m_pkt);
}

bool SoftwareVideoDecoder::initialize(uint32_t codecId) {
    if (m_isInitialized) return true;

    // 1. 查找解码器
    AVCodec* codec = avcodec_find_decoder(codecId);
    if (!codec) {
        std::cerr << "Codec not found for id: " << codecId << std::endl;
        return false;
    }

    // 2. 分配解码器上下文
    m_codecCtx = avcodec_alloc_context3(codec);
    if (!m_codecCtx) {
        std::cerr << "Failed to allocate codec context" << std::endl;
        return false;
    }

    // 工业级做法还需要通过 avcodec_parameters_to_context 把流参数拷贝进来
    // 这里做演示跳过...

    // 3. 打开解码器
    if (avcodec_open2(m_codecCtx, codec, nullptr) < 0) {
        std::cerr << "Failed to open codec" << std::endl;
        avcodec_free_context(&m_codecCtx);
        return false;
    }

    m_isInitialized = true;
    return true;
}

void SoftwareVideoDecoder::destroy() {
    if (m_codecCtx) {
        avcodec_free_context(&m_codecCtx);
    }
    m_isInitialized = false;
}

bool SoftwareVideoDecoder::sendPacket(const uint8_t* data, int size, int64_t pts, int64_t dts) {
    if (!m_isInitialized || !m_codecCtx || !m_pkt) return false;

    // 准备 Packet 数据
    m_pkt->data = const_cast<uint8_t*>(data);
    m_pkt->size = size;
    m_pkt->pts = pts;
    m_pkt->dts = dts;

    // 发送包到解码队列
    int ret = avcodec_send_packet(m_codecCtx, m_pkt);
    if (ret < 0) {
        // 如果返回 EAGAIN 代表解码器输入缓冲区已满，需要先 receiveFrame
        return false;
    }
    return true;
}

bool SoftwareVideoDecoder::receiveFrame(VideoFrame& outFrame) {
    if (!m_isInitialized || !m_codecCtx || !m_frame) return false;

    // 从解码队列获取一帧解码后的画面
    int ret = avcodec_receive_frame(m_codecCtx, m_frame);
    if (ret < 0) {
        // EAGAIN 代表需要送入更多 packet
        // AVERROR_EOF 代表解码器已经输出完所有缓冲的帧
        return false;
    }

    // 映射 AVFrame 到自定义 VideoFrame
    outFrame.width = m_frame->width;
    outFrame.height = m_frame->height;
    outFrame.pts = m_frame->pts;

    for (int i = 0; i < 8; ++i) {
        outFrame.data[i] = m_frame->data[i];
        outFrame.linesize[i] = m_frame->linesize[i];
    }

    return true;
}

} // namespace media
} // namespace video_sdk
