#include "OesToRgbFilter.h"
#include <vector>

namespace video_sdk {
namespace core {

OesToRgbFilter::OesToRgbFilter(std::shared_ptr<rhi::IRenderer> renderer) : m_renderer(renderer) {
    initShader();
}

OesToRgbFilter::~OesToRgbFilter() {
    // RHI 接口会在析构时自动释放资源，无需手动调用 glDeleteProgram
}

void OesToRgbFilter::initShader() {
    const std::string vertexShader = R"(
        attribute vec4 aPosition;
        attribute vec4 aTextureCoord;
        uniform mat4 uTextureMatrix;
        varying vec2 vTextureCoord;
        void main() {
            gl_Position = aPosition;
            vTextureCoord = (uTextureMatrix * aTextureCoord).xy;
        }
    )";

    const std::string fragmentShader = R"(
        #extension GL_OES_EGL_image_external : require
        precision mediump float;
        varying vec2 vTextureCoord;
        uniform samplerExternalOES uTexture;
        void main() {
            gl_FragColor = texture2D(uTexture, vTextureCoord);
        }
    )";

    m_shader = m_renderer->createShader(vertexShader, fragmentShader);
}

void OesToRgbFilter::render(std::shared_ptr<rhi::ITexture> oesTexture, std::shared_ptr<rhi::ITexture> outputTexture, const float* transformMatrix) {
    if (!outputTexture || !oesTexture || !m_shader) return;

    // 1. 设置 RenderTarget (隐藏了 FBO 细节)
    m_renderer->setRenderTarget(outputTexture);
    m_renderer->clear(0.0f, 0.0f, 0.0f, 1.0f);

    // 2. 绑定 Shader
    m_renderer->bindShader(m_shader);

    // 3. 传入矩阵
    m_renderer->setUniformMatrix4fv(m_shader, "uTextureMatrix", transformMatrix);

    // 4. 绑定纹理
    m_renderer->bindTexture(m_shader, "uTexture", oesTexture, 0);

    // 5. 组装顶点数据并绘制 (隐藏了 glDrawArrays 和 VBO/VAO 细节)
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

    // 假设 attr location 0 是 position, 1 是 texcoord
    std::vector<rhi::VertexAttribute> attrs = {
        {0, 2, 0, vertexData},
        {1, 2, 0, textureData}
    };

    m_renderer->drawArrays(5 /* GL_TRIANGLE_STRIP */, 0, 4, attrs);

    // 6. 恢复状态 (可选，RHI内部也可以处理)
    m_renderer->setRenderTarget(nullptr);
}

} // namespace core
} // namespace video_sdk
