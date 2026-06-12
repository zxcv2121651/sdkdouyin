#pragma once
#include <string>
#include <memory>
#include "rhi/interface/IRenderer.h"

namespace video_sdk {
namespace core {

class FilterEngine {
public:
    explicit FilterEngine(std::shared_ptr<rhi::IRenderer> renderer);
    ~FilterEngine();

    // 纯正 RHI 接口
    void applyFilter(const std::string& filterType, std::shared_ptr<rhi::ITexture> inputTexture, std::shared_ptr<rhi::ITexture> outputTexture);

private:
    void initShaders();
    void applyBeautyCompute(std::shared_ptr<rhi::ITexture> inputTexture, std::shared_ptr<rhi::ITexture> outputTexture, float intensity);

private:
    std::shared_ptr<rhi::IRenderer> m_renderer;

    // Shader resources
    std::shared_ptr<rhi::IShader> m_defaultProgram;
    std::shared_ptr<rhi::IShader> m_lutFilterProgram;
    std::shared_ptr<rhi::IShader> m_beautyComputeProgram;
};

} // namespace core
} // namespace video_sdk
