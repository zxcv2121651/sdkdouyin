#pragma once
#include <memory>
#include <string>
#include "rhi/interface/IRenderer.h"

namespace video_sdk {
namespace core {

/**
 * @brief FilterEngine 负责管理特效渲染。
 * 支持片段着色器美颜。
 */
class FilterEngine {
public:
    FilterEngine(std::shared_ptr<rhi::IRenderer> renderer);
    ~FilterEngine();

    void applyFilter(const std::string& filterName, uint32_t inputTextureId, uint32_t outputTextureId, int width, int height);

private:
    std::shared_ptr<rhi::IRenderer> m_renderer;

    uint32_t m_defaultProgram = 0;
    uint32_t m_beautyProgram = 0;

    void initShaders();
};

} // namespace core
} // namespace video_sdk
