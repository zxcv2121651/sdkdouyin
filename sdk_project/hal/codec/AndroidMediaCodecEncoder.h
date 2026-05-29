#pragma once
#include <cstdint>
#include <string>

#ifdef ANDROID
#include <media/NdkMediaCodec.h>
#include <media/NdkMediaFormat.h>
#else
struct AMediaCodec;
struct AMediaFormat;
#endif

namespace video_sdk {
namespace hal {

/**
 * @brief Android 平台的硬件编码器封装。
 * 使用 NDK AMediaCodec，基于 InputSurface 实现渲染即编码的零拷贝导出。
 */
class AndroidMediaCodecEncoder {
public:
    AndroidMediaCodecEncoder();
    ~AndroidMediaCodecEncoder();

    /**
     * 初始化硬件编码器
     * @param codecId 编码格式 (例如，H.264 或 H.265)
     * @param width 输出视频宽
     * @param height 输出视频高
     * @param bitrate 码率
     * @param fps 帧率
     * @return 是否初始化成功
     */
    bool initialize(uint32_t codecId, int width, int height, int bitrate, int fps);

    /**
     * 获取编码器的 Input Surface
     * 渲染引擎可以直接将画面绘制到此 Surface 上，无需 CPU 介入。
     */
    void* getInputSurface() const { return m_inputSurface; }

    /**
     * 提取编码后的数据包并写入文件/复用器
     * 在每一帧渲染完后调用此方法排空输出缓冲。
     */
    void drainOutput();

    void destroy();

private:
    bool m_isInitialized = false;
    AMediaCodec* m_codec = nullptr;
    AMediaFormat* m_format = nullptr;
    void* m_inputSurface = nullptr;
};

} // namespace hal
} // namespace video_sdk
