#pragma once
#include <string>
#include <cstdint>

// 前置声明
struct AVCodecContext;
struct AVFrame;
struct AVPacket;

namespace video_sdk {
namespace media {

// 模拟解码后的视频帧数据
struct VideoFrame {
    uint8_t* data[8];    // 多平面指针 (如 YUV 的 Y/U/V 平面)
    int linesize[8];     // 每行的步长 (stride)
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

    // 根据 CodecId 和额外的参数（如 CodecParameters）初始化解码器
    bool initialize(uint32_t codecId);

    // 释放解码器
    void destroy();

    // 接收解封装器分离出的 Packet，送入解码器
    // data 对应 AVPacket.data
    bool sendPacket(const uint8_t* data, int size, int64_t pts, int64_t dts);

    // 从解码器获取解码后的原始帧 (如 YUV420P)
    bool receiveFrame(VideoFrame& outFrame);

private:
    AVCodecContext* m_codecCtx = nullptr;
    AVFrame* m_frame = nullptr;
    AVPacket* m_pkt = nullptr;
    bool m_isInitialized = false;
};

} // namespace media
} // namespace video_sdk
