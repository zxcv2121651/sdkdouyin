#include "SoftwareVideoDecoder.h"

namespace video_sdk {
namespace media {

SoftwareVideoDecoder::SoftwareVideoDecoder() {}

SoftwareVideoDecoder::~SoftwareVideoDecoder() {
    destroy();
}

bool SoftwareVideoDecoder::initialize(const std::string& codecName) {
    // 1. avcodec_find_decoder 查找对应的解码器（如 h264, hevc）
    // 2. avcodec_alloc_context3 分配上下文
    // 3. avcodec_open2 打开解码器
    m_isInitialized = true;
    return true;
}

void SoftwareVideoDecoder::destroy() {
    // 释放 avcodec_free_context 等资源
    m_isInitialized = false;
}

bool SoftwareVideoDecoder::sendPacket(const uint8_t* data, int size, int64_t pts) {
    if (!m_isInitialized) return false;
    // 调用 avcodec_send_packet
    return true;
}

bool SoftwareVideoDecoder::receiveFrame(VideoFrame& outFrame) {
    if (!m_isInitialized) return false;
    // 调用 avcodec_receive_frame 获取 AVFrame
    // 将 AVFrame 中的 YUV 指针及宽高等数据赋值给 outFrame
    return false;
}

} // namespace media
} // namespace video_sdk
