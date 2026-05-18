#pragma once
#include <string>
#include <memory>

namespace video_sdk {
namespace modules {

/**
 * @brief 特效管理器，负责调度底层的 FilterEngine。
 * 提供上层的滤镜参数设置，如美颜程度调节、LUT 应用。
 */
class EffectManager {
public:
    EffectManager();
    ~EffectManager();

    // 加载全局美颜参数（如：磨皮、瘦脸、大眼）
    void setBeautyParams(float smoothing, float faceSlimming, float eyeEnlarging);

    // 加载特定的 LUT 滤镜（Lookup Table）
    void applyLutFilter(const std::string& lutPath, float intensity);

    // 移除所有特效
    void clearAllEffects();

private:
    float m_smoothingLevel = 0.0f;
    float m_faceSlimmingLevel = 0.0f;
    float m_eyeEnlargingLevel = 0.0f;

    // 依赖注入：指向核心 FilterEngine 的指针
    // std::shared_ptr<core::FilterEngine> m_filterEngine;
};

} // namespace modules
} // namespace video_sdk
