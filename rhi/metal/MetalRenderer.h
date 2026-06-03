#pragma once
#include "IRenderer.h"

namespace video_sdk {
namespace rhi {

/**
 * @brief Metal 的渲染器具体实现骨架 (仅限 Apple 平台)。
 * 高效对接 iOS/macOS 渲染管线。
 */
class MetalRenderer : public IRenderer {
public:
    MetalRenderer();
    ~MetalRenderer() override;

    void initialize() override;
    void destroy() override;

    uint32_t acquireFBO(int width, int height) override;
    void releaseFBO(uint32_t fboId) override;

    uint32_t compileShader(const std::string& vertexSource, const std::string& fragmentSource) override;
};

} // namespace rhi
} // namespace video_sdk
