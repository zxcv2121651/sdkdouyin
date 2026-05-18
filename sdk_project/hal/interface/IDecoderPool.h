#pragma once
#include <string>

namespace video_sdk {
namespace hal {

/**
 * @brief IDecoderPool abstracts hardware decoders (AMediaCodec/AVAssetReader)
 */
class IDecoderPool {
public:
    virtual ~IDecoderPool() = default;

    virtual void requestDecoder(const std::string& assetPath) = 0;
    virtual void releaseDecoder(const std::string& assetPath) = 0;
};

} // namespace hal
} // namespace video_sdk
