#pragma once
#include <memory>
#include <string>
#include "rhi/interface/IRenderer.h"

namespace video_sdk {
namespace core {

/**
 * @brief FilterEngine 负责管理特效渲染。
 * 包含 GLES 3.1+ 基于 Compute Shader 的高阶图像处理逻辑。
 */
class FilterEngine {
public:
    FilterEngine(std::shared_ptr<rhi::IRenderer> renderer);
    ~FilterEngine();

    // 常规 2D 特效滤镜（管线渲染）
    void applyFilter(const std::string& filterName, uint32_t inputTextureId, uint32_t outputTextureId, int width, int height);

    // 高阶美颜滤镜（使用 Compute Shader 进行 Bilateral 磨皮等处理）
    void applyBeautyCompute(uint32_t inputTextureId, uint32_t outputTextureId, int width, int height, float intensity);

private:
    std::shared_ptr<rhi::IRenderer> m_renderer;

    // Shader programs
    uint32_t m_beautyComputeProgram = 0;

    void initShaders();
};

} // namespace core
} // namespace video_sdk
