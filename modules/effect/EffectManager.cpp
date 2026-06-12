#include "EffectManager.h"
#include <iostream>

namespace video_sdk {
namespace modules {

EffectManager::EffectManager(std::shared_ptr<core::FilterEngine> engine) : m_filterEngine(engine) {
}

EffectManager::~EffectManager() {
}

void EffectManager::setBeautyParams(float smoothing, float faceSlimming, float eyeEnlarging) {
    m_smoothingLevel = smoothing;
    m_faceSlimmingLevel = faceSlimming;
    m_eyeEnlargingLevel = eyeEnlarging;
    std::cout << "[EffectManager] Beauty params updated." << std::endl;
}

void EffectManager::applyLutFilter(const std::string& lutPath, float intensity) {
    // 工业级实现：从本地路径加载 LUT png，并作为 3D/2D Texture 注册到 FilterEngine。
    m_activeFilters.push_back({"LUT_" + lutPath, intensity});
    std::cout << "[EffectManager] Applied LUT filter: " << lutPath << std::endl;
}

void EffectManager::clearAllEffects() {
    m_activeFilters.clear();
    m_smoothingLevel = 0;
    m_faceSlimmingLevel = 0;
    m_eyeEnlargingLevel = 0;
}

void EffectManager::processEffects(std::shared_ptr<rhi::ITexture> inputTexture, std::shared_ptr<rhi::ITexture> outputTexture) {
    if (!m_filterEngine || !inputTexture || !outputTexture) return;

    // TODO: In a real implementation with multiple filters, we need a ping-pong buffer mechanism
    // using renderer->createTexture2D(...) to chain multiple filter inputs and outputs.
    // For this mock, we just apply the last one, or apply beauty directly to output.

    std::shared_ptr<rhi::ITexture> currentTexture = inputTexture;

    // 如果启用了美颜（假设美颜是一个 Compute Shader Filter）
    if (m_smoothingLevel > 0.0f) {
        // Here we just apply it directly to output for simplicity of the mock.
        m_filterEngine->applyFilter("Beauty_Compute", currentTexture, outputTexture);
        return;
    }

    // 循环挂载普通的片段着色器滤镜
    for (const auto& config : m_activeFilters) {
        m_filterEngine->applyFilter(config.name, currentTexture, outputTexture);
        return; // Just mock one pass for now
    }

    // 如果没有任何滤镜，只需将 inputTexture 拷贝到 outputTexture (使用一个 copy shader)
    // m_filterEngine->applyFilter("Copy", inputTexture, outputTexture);
}

} // namespace modules
} // namespace video_sdk
