#include "EffectManager.h"

namespace video_sdk {
namespace modules {

EffectManager::EffectManager() {}

EffectManager::~EffectManager() {
    clearAllEffects();
}

void EffectManager::setBeautyParams(float smoothing, float faceSlimming, float eyeEnlarging) {
    m_smoothingLevel = smoothing;
    m_faceSlimmingLevel = faceSlimming;
    m_eyeEnlargingLevel = eyeEnlarging;
    // 将参数传递给 FilterEngine 中对应的美颜 Shader Uniforms
}

void EffectManager::applyLutFilter(const std::string& lutPath, float intensity) {
    // 1. 读取并解码 LUT 图像
    // 2. 作为 3D Texture 或者是 2D Texture(平铺版) 上传至 GPU
    // 3. 配置 FilterEngine 使用 LUT Filter 的渲染路径
}

void EffectManager::clearAllEffects() {
    // 通知 FilterEngine 清空挂载的滤镜队列，只保留原图拷贝或 OES 转 RGB 逻辑
}

} // namespace modules
} // namespace video_sdk
