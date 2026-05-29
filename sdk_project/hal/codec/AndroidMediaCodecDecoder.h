#pragma once
#include <cstdint>
#include <string>

#ifdef ANDROID
#include <media/NdkMediaCodec.h>
#include <media/NdkMediaFormat.h>
#else
// 占位定义，防止在非 Android 环境下编译报错
struct AMediaCodec;
struct AMediaFormat;
#endif

namespace video_sdk {
namespace hal {

/**
 * @brief Android 平台的硬件解码器封装。
 * 使用 NDK AMediaCodec 接口，实现 H.264/H.265 的硬解。
 * 支持直接输出到 Surface (零拷贝模式)。
 */
class AndroidMediaCodecDecoder {
public:
    AndroidMediaCodecDecoder();
    ~AndroidMediaCodecDecoder();

    /**
     * 初始化解码器
     * @param codecId 编码格式 ID (例如，区分 H.264 和 H.265)
     * @param width 视频宽
     * @param height 视频高
     * @param nativeWindow 绑定的输出目标 (Surface)
     * @return 是否初始化成功
     */
    bool initialize(uint32_t codecId, int width, int height, void* nativeWindow);

    /**
     * 销毁并释放解码器资源
     */
    void destroy();

    /**
     * 发送编码数据包到解码器
     */
    bool sendPacket(const uint8_t* data, int size, int64_t pts);

    /**
     * 接收解码后的帧 (在零拷贝模式下，直接渲染到绑定的 Surface 上)
     */
    bool receiveFrame();

private:
    bool m_isInitialized = false;
    AMediaCodec* m_codec = nullptr;
    AMediaFormat* m_format = nullptr;
};

} // namespace hal
} // namespace video_sdk
