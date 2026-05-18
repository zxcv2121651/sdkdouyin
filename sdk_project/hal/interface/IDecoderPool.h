#pragma once
#include <string>

namespace video_sdk {
namespace hal {

/**
 * @brief IDecoderPool 用于抽象硬件解码器 (如 Android 的 AMediaCodec 或 iOS 的 AVAssetReader)
 */
class IDecoderPool {
public:
    virtual ~IDecoderPool() = default;

    // 请求解码器资源
    virtual void requestDecoder(const std::string& assetPath) = 0;
    // 释放解码器资源
    virtual void releaseDecoder(const std::string& assetPath) = 0;
};

} // namespace hal
} // namespace video_sdk
