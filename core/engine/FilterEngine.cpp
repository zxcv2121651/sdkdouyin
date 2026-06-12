#include "FilterEngine.h"
#include <iostream>
#include <vector>

namespace video_sdk {
namespace core {

FilterEngine::FilterEngine(std::shared_ptr<rhi::IRenderer> renderer) : m_renderer(renderer) {
    initShaders();
}

FilterEngine::~FilterEngine() {
}

void FilterEngine::initShaders() {
    const std::string defaultVs = R"(
        attribute vec4 aPosition;
        attribute vec2 aTexCoord;
        varying vec2 vTexCoord;
        void main() {
            gl_Position = aPosition;
            vTexCoord = aTexCoord;
        }
    )";

    const std::string defaultFs = R"(
        precision mediump float;
        varying vec2 vTexCoord;
        uniform sampler2D uTexture;
        void main() {
            gl_FragColor = texture2D(uTexture, vTexCoord);
        }
    )";

    m_defaultProgram = m_renderer->createShader(defaultVs, defaultFs);

    // Mock Compute Shader setup
    const std::string beautyComputeSrc = R"(
        #version 310 es
        layout(local_size_x = 16, local_size_y = 16) in;
        // Mock Implementation
        void main() {}
    )";
    m_beautyComputeProgram = m_renderer->createComputeShader(beautyComputeSrc);
}

void FilterEngine::applyFilter(const std::string& filterType, std::shared_ptr<rhi::ITexture> inputTexture, std::shared_ptr<rhi::ITexture> outputTexture) {
    if (!inputTexture || !outputTexture) return;

    if (filterType == "Beauty_Compute" && m_beautyComputeProgram) {
        applyBeautyCompute(inputTexture, outputTexture, 0.8f);
        return;
    }

    m_renderer->setRenderTarget(outputTexture);
    m_renderer->clear(0.0f, 0.0f, 0.0f, 1.0f);

    std::shared_ptr<rhi::IShader> activeShader = m_defaultProgram; // fall back to copy if not found
    m_renderer->bindShader(activeShader);
    m_renderer->bindTexture(activeShader, "uTexture", inputTexture, 0);

    // Mock Vertex Data
    static const float vertexData[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f
    };
    static const float textureData[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f
    };

    std::vector<rhi::VertexAttribute> attrs = {
        {0, 2, 0, vertexData},
        {1, 2, 0, textureData}
    };

    m_renderer->drawArrays(5, 0, 4, attrs);
}

void FilterEngine::applyBeautyCompute(std::shared_ptr<rhi::ITexture> inputTexture, std::shared_ptr<rhi::ITexture> outputTexture, float intensity) {
    if (!m_beautyComputeProgram) return;

    int width = outputTexture->getWidth();
    int height = outputTexture->getHeight();

    int groupX = (width + 15) / 16;
    int groupY = (height + 15) / 16;

    m_renderer->bindShader(m_beautyComputeProgram);
    // In pure RHI, bindTexture for compute shader image load/store
    m_renderer->bindTexture(m_beautyComputeProgram, "uInputImage", inputTexture, 0);
    m_renderer->bindTexture(m_beautyComputeProgram, "uOutputImage", outputTexture, 1);

    m_renderer->dispatchCompute(m_beautyComputeProgram, groupX, groupY, 1);
}

} // namespace core
} // namespace video_sdk
