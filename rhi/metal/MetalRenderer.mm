#include "MetalRenderer.h"

namespace video_sdk {
namespace rhi {

MetalRenderer::MetalRenderer() {}
MetalRenderer::~MetalRenderer() {
    destroy();
}

void MetalRenderer::initialize() {
    // 获取 MTLDevice，配置 CommandQueue
}

void MetalRenderer::destroy() {
    // 释放 Metal 资源
}

uint32_t MetalRenderer::acquireFBO(int width, int height) {
    // Metal 中通常对应 MTLTexture 或 CVPixelBuffer 缓存池
    return 0;
}

void MetalRenderer::releaseFBO(uint32_t fboId) {
    // 归还缓存资源
}

uint32_t MetalRenderer::compileShader(const std::string& vertexSource, const std::string& fragmentSource) {
    // 编译 Metal 着色语言 (MSL) 为 MTLLibrary
    return 0;
}

} // namespace rhi
} // namespace video_sdk
