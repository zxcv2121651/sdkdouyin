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
 * @brief 顶级特效业务管家 (Effect Manager)。
 *
 * 核心原理：
 * C++ 底层的 `FilterEngine` 只负责拿着 Shader 算像素。
 * 它是没有“美白”、“大眼”这些业务概念的，它只知道 Uniform 和 Texture。
 *
 * 本类负责将人类能听懂的“瘦脸 50%”，翻译为底层能看懂的矩阵变换和 Shader 参数。
 * 在复杂的 SDK 中，这里通常会接入类似于 AI 人脸关键点识别库，
 * 把人脸 106 个点坐标计算成 Mesh 扔给底层去发生形变。
 */
class EffectManager {
public:
    EffectManager(std::shared_ptr<core::FilterEngine> engine);
    ~EffectManager();

    /**
     * @brief 设置全局美颜参数
     * @param smoothing 磨皮程度 (通常对应双边滤波/高斯模糊的半径和权重)
     * @param faceSlimming 瘦脸程度 (对应形变 Mesh 的液化强度)
     * @param eyeEnlarging 大眼程度 (对应局部放大的 Shader 采样偏移)
     */
    void setBeautyParams(float smoothing, float faceSlimming, float eyeEnlarging);

    /**
     * @brief 应用风格滤镜 (例如“赛博朋克”、“日系清新”)
     * @param lutPath 颜色查找表 (Lookup Table) 图片文件的绝对路径
     * @param intensity 滤镜混合浓度 0.0 ~ 1.0 (在底层的 Fragment Shader 中用 mix 函数计算)
     */
    void applyLutFilter(const std::string& lutPath, float intensity);

    // 移除所有的挂载特效，恢复原画
    void clearAllEffects();

    /**
     * @brief 触发执行特效链。
     * 通常由 `RenderNode` (比如 `FilterNode`) 在其 `process()` 中被调用。
     */
    void processEffects(uint32_t inputTextureId, uint32_t outputTextureId, int width, int height);

private:
    float m_smoothingLevel = 0.0f;
    float m_faceSlimmingLevel = 0.0f;
    float m_eyeEnlargingLevel = 0.0f;

    std::shared_ptr<core::FilterEngine> m_filterEngine;

    // 顺序挂载的滤镜执行队列
    std::vector<FilterConfig> m_activeFilters;
};

} // namespace modules
} // namespace video_sdk
