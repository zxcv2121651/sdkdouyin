#pragma once
#include <memory>
#include <string>
#include <unordered_map>

namespace video_sdk {
namespace core {

/**
 * @brief FilterEngine manages 100% of the effect rendering.
 * Operates on the RHI level and ensures Zero-Copy when possible.
 */
class FilterEngine {
public:
    FilterEngine();
    ~FilterEngine();

    void applyFilter(const std::string& filterName, uint32_t inputTextureId, uint32_t outputTextureId);

private:
    // Manage shader states and FBO pools
};

} // namespace core
} // namespace video_sdk
