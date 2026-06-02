#pragma once
#include <string>
#include <cstdint>

// 前置声明 FFmpeg 核心结构
struct AVCodecContext;
struct AVFrame;
struct AVPacket;

namespace video_sdk {
namespace media {

/**
 * @brief 框架内部映射的解码帧结构 (VideoFrame)。
 * 用于承载解码后存放在 CPU 内存中的原始像素数据 (通常是 YUV420P 格式)。
 */
struct VideoFrame {
    uint8_t* data[8];    // 多平面指针 (例如 data[0]=Y, data[1]=U, data[2]=V)
    int linesize[8];     // 每行的步长 (Stride，注意它可能大于 width 以对齐内存)
    int width;
    int height;
    int64_t pts;         // 该帧应当在何时显示 (Presentation Time Stamp)
};

/**
 * @brief 基于 FFmpeg (libavcodec) 的 CPU 软件视频解码器。
 *
 * 架构定位：
 * 它是 `AndroidMediaCodecDecoder` (硬解) 的替补方案。
 * 当用户导入的视频是古怪的编码格式（如 VP9、GIF 等手机硬件解码器不支持的格式）时，
 * `DecoderPool` 会自动降级调用此软件解码器。
 * 软解的缺点是极其消耗 CPU，且解码出的 YUV 必须经过 `glTexImage2D` 拷贝到显存才能渲染。
 */
class SoftwareVideoDecoder {
public:
    SoftwareVideoDecoder();
    ~SoftwareVideoDecoder();

    /**
     * @brief 根据格式 ID 初始化 FFmpeg 解码上下文。
     * @param codecId FFmpeg 定义的编码格式 ID，例如 AV_CODEC_ID_H264
     */
    bool initialize(uint32_t codecId);

    // 释放解码器及上下文内存
    void destroy();

    /**
     * @brief 将压缩的数据包推入解码器内部缓冲队列。
     * 工业级 FFmpeg 3.x 之后的 API 采用异步解耦模式，`sendPacket` 不会立即出结果。
     *
     * @param data 压缩流数据指针
     * @param size 数据长度
     * @param pts 显示时间
     * @param dts 解码时间 (B帧存在时，DTS 会与 PTS 不同)
     */
    bool sendPacket(const uint8_t* data, int size, int64_t pts, int64_t dts);

    /**
     * @brief 从解码器内部的输出队列中捞取一帧已经解好压的原始画面。
     * 如果返回 false 代表需要调用方继续 `sendPacket` 塞入更多数据。
     *
     * @param outFrame [输出] 承载解码后 YUV 像素的载体
     */
    bool receiveFrame(VideoFrame& outFrame);

private:
    AVCodecContext* m_codecCtx = nullptr;
    AVFrame* m_frame = nullptr;
    AVPacket* m_pkt = nullptr;
    bool m_isInitialized = false;
};

} // namespace media
} // namespace video_sdk
