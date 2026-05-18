#pragma once
#include <string>
#include <cstdint>

namespace video_sdk {
namespace media {

// 模拟解码后的视频帧数据
struct VideoFrame {
    uint8_t* yData;
    uint8_t* uData;
    uint8_t* vData;
    int width;
    int height;
    int64_t pts;
};

/**
 * @brief 基于 FFmpeg libavcodec 的软件视频解码器。
 * 针对硬件解码回退或不支持的格式（如特殊的动画格式），使用 CPU 进行解码。
 */
class SoftwareVideoDecoder {
public:
    SoftwareVideoDecoder();
    ~SoftwareVideoDecoder();

    // 初始化解码器，传入流信息中的解码器上下文参数
    bool initialize(const std::string& codecName);

    // 释放解码器
    void destroy();

    // 接收解封装器分离出的 Packet，送入解码器 (avcodec_send_packet)
    bool sendPacket(const uint8_t* data, int size, int64_t pts);

    // 从解码器获取解码后的原始 YUV 帧 (avcodec_receive_frame)
    // 返回值表示是否成功获取到帧
    bool receiveFrame(VideoFrame& outFrame);

private:
    // FFmpeg 结构体占位: AVCodecContext*, AVCodec*, AVFrame* 等
    bool m_isInitialized = false;
};

} // namespace media
} // namespace video_sdk
