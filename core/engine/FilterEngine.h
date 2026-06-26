#pragma once
#include <memory>
#include <string>
#include "rhi/interface/IRenderer.h"

namespace video_sdk {
namespace core {

/**
 * @brief FilterEngine 负责管理特效渲染。
 * 现代架构：底线 GLES 3.1+。高级算法全面拥抱 Compute Shader。
 */
class FilterEngine {
public:
    FilterEngine(std::shared_ptr<rhi::IRenderer> renderer);
    ~FilterEngine();

    // 常规 2D 特效滤镜（管线渲染）
    void applyFilter(const std::string& filterName, uint32_t inputTextureId, uint32_t outputTextureId, int width, int height);

    // 高阶计算特效（管线渲染）
    void applyBeautyCompute(uint32_t inputTextureId, uint32_t outputTextureId, int width, int height, float intensity);

private:
    std::shared_ptr<rhi::IRenderer> m_renderer;

    uint32_t m_beautyComputeProgram = 0;
    uint32_t m_defaultProgram = 0;

    void initShaders();
};

} // namespace core
} // namespace video_sdk
