#pragma once

#include <string>
#include <memory>
#include <vector>

namespace video_sdk {
namespace rhi {

// RHI 资源抽象
enum class TextureFormat {
    RGBA8,
    OES_EXTERNAL
};

class ITexture {
public:
    virtual ~ITexture() = default;
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    virtual TextureFormat getFormat() const = 0;
    virtual uint32_t getNativeId() const = 0; // 仅供 RHI 内部使用，Core 层不应调用
};

class IBuffer {
public:
    virtual ~IBuffer() = default;
};

class IShader {
public:
    virtual ~IShader() = default;
};

// 顶点布局抽象
struct VertexAttribute {
    int index;
    int size; // components (e.g. 2 for vec2)
    int stride;
    const void* pointer;
};

/**
 * @brief 纯正的 RHI (Render Hardware Interface)。
 * 担任 Device (资源创建) 与 CommandContext (绘制执行) 双重职责。
 * Core 层只能看到此接口，绝对不允许包含任何 gl/vk/metal 头文件。
 */
class IRenderer {
public:
    virtual ~IRenderer() = default;

    // ==========================================
    // 资源创建 (Device 职责)
    // ==========================================

    // 创建普通 2D 纹理 (作为 FBO 的挂载点)
    virtual std::shared_ptr<ITexture> createTexture2D(int width, int height, TextureFormat format) = 0;

    // 包装外部 OES 纹理 (Kotlin 传进来的 ID)
    virtual std::shared_ptr<ITexture> wrapExternalOESTexture(uint32_t textureId, int width, int height) = 0;

    // 编译着色器
    virtual std::shared_ptr<IShader> createShader(const std::string& vertexSource, const std::string& fragmentSource) = 0;

    // ==========================================
    // 渲染上下文与绘制 (Command Context 职责)
    // ==========================================

    // 绑定渲染目标 (替代 glBindFramebuffer)
    virtual void setRenderTarget(std::shared_ptr<ITexture> texture) = 0;

    // 清屏 (替代 glClear)
    virtual void clear(float r, float g, float b, float a) = 0;

    // 绑定着色器
    virtual void bindShader(std::shared_ptr<IShader> shader) = 0;

    // 绑定纹理到特定槽位 (替代 glActiveTexture + glBindTexture)
    virtual void bindTexture(std::shared_ptr<IShader> shader, const std::string& uniformName, std::shared_ptr<ITexture> texture, int slot) = 0;

    // 更新 Uniform 矩阵 (替代 glUniformMatrix4fv)
    virtual void setUniformMatrix4fv(std::shared_ptr<IShader> shader, const std::string& name, const float* matrix) = 0;

    // 绘制图元 (替代 glDrawArrays)
    virtual void drawArrays(int mode, int first, int count, const std::vector<VertexAttribute>& attributes) = 0;

    // 将最终画面渲染到屏幕 Window
    virtual void presentToScreen() = 0;

    // 获取渲染器类型
    virtual std::string getRendererType() const = 0;

    // ==========================================
    // Compute Shader 支持 (Vulkan/GLES3.1)
    // ==========================================
    virtual std::shared_ptr<IShader> createComputeShader(const std::string& computeSource) { return nullptr; }
    virtual void dispatchCompute(std::shared_ptr<IShader> shader, int numGroupsX, int numGroupsY, int numGroupsZ) {}
};

} // namespace rhi
} // namespace video_sdk
