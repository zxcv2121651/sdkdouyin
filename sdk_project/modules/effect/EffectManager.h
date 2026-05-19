#pragma once
#include <string>
#include <memory>
#include <vector>
#include "core/engine/FilterEngine.h"

namespace video_sdk {
namespace modules {

struct FilterConfig {
    std::string name;
    float intensity;
};

/**
 * @brief 特效管理器，负责调度底层的 FilterEngine。
 * 提供上层的滤镜参数设置，如美颜程度调节、LUT 应用。
 */
class EffectManager {
public:
    EffectManager(std::shared_ptr<core::FilterEngine> engine);
    ~EffectManager();

    // 加载全局美颜参数（如：磨皮、瘦脸、大眼）
    void setBeautyParams(float smoothing, float faceSlimming, float eyeEnlarging);

    // 加载特定的 LUT 滤镜（Lookup Table）
    void applyLutFilter(const std::string& lutPath, float intensity);

    // 移除所有特效
    void clearAllEffects();

    // 被 RenderGraph 或 Player 调用：依次执行已挂载的特效
    void processEffects(uint32_t inputTextureId, uint32_t outputTextureId, int width, int height);

private:
    float m_smoothingLevel = 0.0f;
    float m_faceSlimmingLevel = 0.0f;
    float m_eyeEnlargingLevel = 0.0f;

    std::shared_ptr<core::FilterEngine> m_filterEngine;

    // 挂载的滤镜队列
    std::vector<FilterConfig> m_activeFilters;
};

} // namespace modules
} // namespace video_sdk
