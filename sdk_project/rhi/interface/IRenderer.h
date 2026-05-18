#pragma once
#include <cstdint>
#include <string>

namespace video_sdk {
namespace rhi {

/**
 * @brief IRenderer is the hardware abstraction layer for rendering.
 * Provides unified interfaces for OpenGL ES, Vulkan, and Metal.
 */
class IRenderer {
public:
    virtual ~IRenderer() = default;

    virtual void initialize() = 0;
    virtual void destroy() = 0;

    // FBO Management
    virtual uint32_t acquireFBO(int width, int height) = 0;
    virtual void releaseFBO(uint32_t fboId) = 0;

    // Shader Management
    virtual uint32_t compileShader(const std::string& vertexSource, const std::string& fragmentSource) = 0;
};

} // namespace rhi
} // namespace video_sdk
