#pragma once
#include <cstdint>
#include <string>

namespace video_sdk {
namespace rhi {

/**
 * @brief IRenderer 是渲染引擎的硬件抽象层（HAL）。
 * 为 OpenGL ES、Vulkan 和 Metal 提供统一的操作接口。
 */
class IRenderer {
public:
    virtual ~IRenderer() = default;

    virtual void initialize() = 0;
    virtual void destroy() = 0;

    // FBO (帧缓冲对象) 管理
    virtual uint32_t acquireFBO(int width, int height) = 0;
    virtual void releaseFBO(uint32_t fboId) = 0;

    // Shader (着色器) 管理
    virtual uint32_t compileShader(const std::string& vertexSource, const std::string& fragmentSource) = 0;
};

} // namespace rhi
} // namespace video_sdk
