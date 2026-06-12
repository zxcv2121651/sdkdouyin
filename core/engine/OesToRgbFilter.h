#pragma once
#include "rhi/interface/IRenderer.h"
#include <memory>
#include <string>

namespace video_sdk {
namespace core {

/**
 * @brief 将 OES 纹理转换为标准 RGB 纹理的滤镜。
 * 纯正 RHI 架构版本：没有任何 gl 头文件和宏。
 */
class OesToRgbFilter {
public:
    explicit OesToRgbFilter(std::shared_ptr<rhi::IRenderer> renderer);
    ~OesToRgbFilter();

    // 核心绘制接口：接收 OESTexture，输出到 outputTexture
    void render(std::shared_ptr<rhi::ITexture> oesTexture, std::shared_ptr<rhi::ITexture> outputTexture, const float* transformMatrix);

private:
    void initShader();

private:
    std::shared_ptr<rhi::IRenderer> m_renderer;
    std::shared_ptr<rhi::IShader> m_shader;
};

} // namespace core
} // namespace video_sdk
