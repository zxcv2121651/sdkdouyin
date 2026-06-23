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
    m_activeFilters.push_back({"LUT_" + lutPath, intensity});
    std::cout << "[EffectManager] Applied LUT filter: " << lutPath << std::endl;
}

void EffectManager::clearAllEffects() {
    m_activeFilters.clear();
    m_smoothingLevel = 0;
    m_faceSlimmingLevel = 0;
    m_eyeEnlargingLevel = 0;
}

void EffectManager::processEffects(uint32_t inputTextureId, uint32_t outputTextureId, int width, int height) {
    if (!m_filterEngine || inputTextureId == 0) return;

    uint32_t currentTexture = inputTextureId;

    if (m_smoothingLevel > 0.0f) {
        m_filterEngine->applyFilter("BeautyFilter", currentTexture, outputTextureId, width, height);
        return;
    }

    for (const auto& config : m_activeFilters) {
        m_filterEngine->applyFilter(config.name, currentTexture, outputTextureId, width, height);
        return;
    }
}

} // namespace modules
} // namespace video_sdk
