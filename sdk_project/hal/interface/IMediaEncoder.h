#pragma once
#include <cstdint>
#include <string>

namespace video_sdk {
namespace hal {

/**
 * @brief 硬件视频编码器抽象接口。
 * 用于录制 (Record) 和导出 (Export) 阶段。
 */
class IMediaEncoder {
public:
    virtual ~IMediaEncoder() = default;

    /**
     * @brief 初始化编码器
     * @param outputPath 输出文件路径 (如 .mp4)
     * @param width 视频宽
     * @param height 视频高
     * @param fps 帧率
     * @param bitrate 码率
     * @return 成功返回 true
     */
    virtual bool initialize(const std::string& outputPath, int width, int height, int fps, int bitrate) = 0;

    /**
     * @brief 启动编码器
     */
    virtual void start() = 0;

    /**
     * @brief 停止编码器并写入文件尾
     */
    virtual void stop() = 0;

    /**
     * @brief 送入一帧数据 (YUV 数据或纹理 ID)
     * @param data YUV数据指针
     * @param size 数据大小
     * @param pts 呈现时间戳 (微秒)
     */
    virtual void encodeFrame(const uint8_t* data, size_t size, int64_t pts) = 0;

    /**
     * @brief 送入一帧纹理数据 (硬件编码器常用)
     * @param textureId 纹理ID
     * @param pts 呈现时间戳 (微秒)
     */
    virtual void encodeTexture(uint32_t textureId, int64_t pts) = 0;
};

} // namespace hal
} // namespace video_sdk
