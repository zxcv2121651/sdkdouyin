#include "EffectManager.h"
#include <iostream>

namespace video_sdk {
namespace modules {

EffectManager::EffectManager(std::shared_ptr<core::FilterEngine> engine) : m_filterEngine(engine) {}

EffectManager::~EffectManager() {
    clearAllEffects();
}

void EffectManager::setBeautyParams(float smoothing, float faceSlimming, float eyeEnlarging) {
    m_smoothingLevel = smoothing;
    m_faceSlimmingLevel = faceSlimming;
    m_eyeEnlargingLevel = eyeEnlarging;
    std::cout << "[EffectManager] Updated Beauty Params: smoothing=" << smoothing << std::endl;
}

void EffectManager::applyLutFilter(const std::string& lutPath, float intensity) {
    // 工业级实现：读取 lutPath，解码图片，上传为 3D Texture 并返回 textureId
    // 这里将其作为记录加入配置队列
    m_activeFilters.push_back({lutPath, intensity});
    std::cout << "[EffectManager] Added LUT Filter: " << lutPath << " with intensity " << intensity << std::endl;
}

void EffectManager::clearAllEffects() {
    m_activeFilters.clear();
    m_smoothingLevel = 0.0f;
    m_faceSlimmingLevel = 0.0f;
    m_eyeEnlargingLevel = 0.0f;
}

void EffectManager::processEffects(uint32_t inputTextureId, uint32_t outputTextureId, int width, int height) {
    if (!m_filterEngine) return;

    // 1. 如果开启了美颜，先过 Compute Shader 磨皮
    uint32_t currentTexture = inputTextureId;

    if (m_smoothingLevel > 0.01f) {
        // 由于需要中间流转，实际可能需要申请临时的 ping-pong FBO
        m_filterEngine->applyBeautyCompute(currentTexture, outputTextureId, width, height, m_smoothingLevel);
        currentTexture = outputTextureId; // 简易状态流转
    }

    // 2. 依次叠加滤镜
    for (const auto& config : m_activeFilters) {
        m_filterEngine->applyFilter(config.name, currentTexture, outputTextureId, width, height);
        currentTexture = outputTextureId;
    }
}

} // namespace modules
} // namespace video_sdk
