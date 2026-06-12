#pragma once
#include "rhi/interface/IRenderer.h"
#include <map>
#include <string>

namespace video_sdk {
namespace rhi {

class GLESShader : public IShader {
public:
    uint32_t programId;
    std::map<std::string, int> uniformLocations;
    std::map<std::string, int> attribLocations;
    ~GLESShader();
};

class GLESTexture : public ITexture {
public:
    uint32_t textureId;
    int width;
    int height;
    TextureFormat format;
    uint32_t fboId = 0; // 如果用作 RenderTarget，顺便持有一个 FBO

    GLESTexture(int w, int h, TextureFormat fmt);
    ~GLESTexture() override;

    int getWidth() const override { return width; }
    int getHeight() const override { return height; }
    TextureFormat getFormat() const override { return format; }
    uint32_t getNativeId() const override { return textureId; }
};

class GLESRenderer : public IRenderer {
public:
    GLESRenderer();
    ~GLESRenderer() override;

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
    std::string getRendererType() const override { return "OpenGLES"; }

private:
    uint32_t compileShaderInternal(uint32_t type, const std::string& source);
};

} // namespace rhi
} // namespace video_sdk
