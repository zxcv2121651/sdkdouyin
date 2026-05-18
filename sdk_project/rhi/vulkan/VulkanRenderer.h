#pragma once
#include "IRenderer.h"

namespace video_sdk {
namespace rhi {

/**
 * @brief Vulkan 的渲染器具体实现骨架。
 * 针对高性能、低开销的跨平台图形渲染。
 */
class VulkanRenderer : public IRenderer {
public:
    VulkanRenderer();
    ~VulkanRenderer() override;

    void initialize() override;
    void destroy() override;

    uint32_t acquireFBO(int width, int height) override;
    void releaseFBO(uint32_t fboId) override;

    uint32_t compileShader(const std::string& vertexSource, const std::string& fragmentSource) override;
};

} // namespace rhi
} // namespace video_sdk
