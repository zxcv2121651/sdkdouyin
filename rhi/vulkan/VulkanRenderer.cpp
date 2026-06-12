#include "VulkanRenderer.h"
#include <iostream>

namespace video_sdk {
namespace rhi {

VulkanRenderer::VulkanRenderer() {
    std::cout << "[VulkanRenderer] Initialized." << std::endl;
}

VulkanRenderer::~VulkanRenderer() {}

std::shared_ptr<ITexture> VulkanRenderer::createTexture2D(int width, int height, TextureFormat format) {
    return std::make_shared<VulkanTexture>(width, height, format);
}

std::shared_ptr<ITexture> VulkanRenderer::wrapExternalOESTexture(uint32_t textureId, int width, int height) {
    auto tex = std::make_shared<VulkanTexture>(width, height, TextureFormat::OES_EXTERNAL);
    tex->id = textureId;
    return tex;
}

std::shared_ptr<IShader> VulkanRenderer::createShader(const std::string& vertexSource, const std::string& fragmentSource) {
    return std::make_shared<VulkanShader>();
}

void VulkanRenderer::setRenderTarget(std::shared_ptr<ITexture> texture) {}
void VulkanRenderer::clear(float r, float g, float b, float a) {}
void VulkanRenderer::bindShader(std::shared_ptr<IShader> shader) {}
void VulkanRenderer::bindTexture(std::shared_ptr<IShader> shader, const std::string& uniformName, std::shared_ptr<ITexture> texture, int slot) {}
void VulkanRenderer::setUniformMatrix4fv(std::shared_ptr<IShader> shader, const std::string& name, const float* matrix) {}
void VulkanRenderer::drawArrays(int mode, int first, int count, const std::vector<VertexAttribute>& attributes) {}
void VulkanRenderer::presentToScreen() {}

} // namespace rhi
} // namespace video_sdk
