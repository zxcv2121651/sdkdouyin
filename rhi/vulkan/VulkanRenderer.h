#pragma once
#include "rhi/interface/IRenderer.h"
#include <string>

namespace video_sdk {
namespace rhi {

class VulkanShader : public IShader {
public:
    uint32_t programId = 0;
};

class VulkanTexture : public ITexture {
public:
    int width;
    int height;
    TextureFormat format;
    uint32_t id;

    VulkanTexture(int w, int h, TextureFormat fmt) : width(w), height(h), format(fmt), id(1) {}
    int getWidth() const override { return width; }
    int getHeight() const override { return height; }
    TextureFormat getFormat() const override { return format; }
    uint32_t getNativeId() const override { return id; }
};

class VulkanRenderer : public IRenderer {
public:
    VulkanRenderer();
    ~VulkanRenderer() override;

    std::shared_ptr<ITexture> createTexture2D(int width, int height, TextureFormat format) override;
    std::shared_ptr<ITexture> wrapExternalOESTexture(uint32_t textureId, int width, int height) override;
    std::shared_ptr<IShader> createShader(const std::string& vertexSource, const std::string& fragmentSource) override;

    void setRenderTarget(std::shared_ptr<ITexture> texture) override;
    void clear(float r, float g, float b, float a) override;
    void bindShader(std::shared_ptr<IShader> shader) override;
    void bindTexture(std::shared_ptr<IShader> shader, const std::string& uniformName, std::shared_ptr<ITexture> texture, int slot) override;
    void setUniformMatrix4fv(std::shared_ptr<IShader> shader, const std::string& name, const float* matrix) override;
    void drawArrays(int mode, int first, int count, const std::vector<VertexAttribute>& attributes) override;

    void presentToScreen() override;
    std::string getRendererType() const override { return "Vulkan"; }
};

} // namespace rhi
} // namespace video_sdk
