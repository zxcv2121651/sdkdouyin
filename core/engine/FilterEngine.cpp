#include "FilterEngine.h"
#include <iostream>

extern void glUseProgram(uint32_t program);
extern int glGetAttribLocation(uint32_t program, const char* name);
extern int glGetUniformLocation(uint32_t program, const char* name);
extern void glUniform1i(int location, int v0);
extern void glUniform1f(int location, float v0);
extern void glUniform2f(int location, float v0, float v1);
extern void glActiveTexture(uint32_t texture);
extern void glBindTexture(uint32_t target, uint32_t texture);
extern void glViewport(int x, int y, int width, int height);
extern void glDrawArrays(uint32_t mode, int first, int count);
extern void glBindFramebuffer(uint32_t target, uint32_t framebuffer);
extern void glEnableVertexAttribArray(uint32_t index);
extern void glDisableVertexAttribArray(uint32_t index);
extern void glVertexAttribPointer(uint32_t index, int size, uint32_t type, bool normalized, int stride, const void* pointer);

#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE_2D 0x0DE1
#define GL_FRAMEBUFFER 0x8D40
#define GL_TRIANGLE_STRIP 0x0005
#define GL_FLOAT 0x1406

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
    if (m_renderer) {
        m_defaultProgram = m_renderer->compileShader(defaultVs, defaultFs);
    }

    // ---------------------------------------------------------
    // 高级美颜 Shader (Bilateral Filter 变体，针对肤色进行保边模糊)
    // ---------------------------------------------------------
    const std::string beautyFs = R"(
        precision mediump float;
        varying vec2 vTexCoord;
        uniform sampler2D uTexture;
        uniform vec2 uTexelSize;
        uniform float uIntensity;

        const int RADIUS = 4;
        const float SIGMA_S = 4.0;
        const float SIGMA_R = 0.1;

        float normpdf(float x, float sigma) {
            return 0.39894 * exp(-0.5 * x * x / (sigma * sigma)) / sigma;
        }

        void main() {
            vec3 centerColor = texture2D(uTexture, vTexCoord).rgb;

            vec3 sumColor = vec3(0.0);
            float sumWeight = 0.0;

            for (int i = -RADIUS; i <= RADIUS; i++) {
                for (int j = -RADIUS; j <= RADIUS; j++) {
                    vec2 offset = vec2(float(i), float(j)) * uTexelSize;
                    vec3 sampleColor = texture2D(uTexture, vTexCoord + offset).rgb;

                    float distSpace = length(vec2(float(i), float(j)));
                    float weightSpace = normpdf(distSpace, SIGMA_S);

                    float distColor = distance(centerColor, sampleColor);
                    float weightColor = normpdf(distColor, SIGMA_R);

                    float weight = weightSpace * weightColor;

                    sumColor += sampleColor * weight;
                    sumWeight += weight;
                }
            }

            vec3 smoothedColor = sumColor / sumWeight;
            vec3 finalColor = mix(centerColor, smoothedColor, uIntensity);

            gl_FragColor = vec4(finalColor, 1.0);
        }
    )";

    if (m_renderer) {
        m_beautyProgram = m_renderer->compileShader(defaultVs, beautyFs);
    }
}

void FilterEngine::applyFilter(const std::string& filterName, uint32_t inputTextureId, uint32_t outputTextureId, int width, int height) {
    if (!m_renderer) return;

    auto fbo = m_renderer->acquireFBO(width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo->fboId);
    glViewport(0, 0, width, height);

    uint32_t activeProgram = m_defaultProgram;
    if (filterName == "BeautyFilter" && m_beautyProgram > 0) {
        activeProgram = m_beautyProgram;
        std::cout << "[FilterEngine] Applying Bilateral Beauty Shader." << std::endl;
    } else {
        std::cout << "[FilterEngine] Applying Default 2D Filter." << std::endl;
    }

    glUseProgram(activeProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTextureId);

    // int texLoc = glGetUniformLocation(activeProgram, "uTexture");
    // glUniform1i(texLoc, 0);

    if (filterName == "BeautyFilter") {
        float texelSizeX = 1.0f / width;
        float texelSizeY = 1.0f / height;
        // int texelSizeLoc = glGetUniformLocation(activeProgram, "uTexelSize");
        // glUniform2f(texelSizeLoc, texelSizeX, texelSizeY);

        // int intensityLoc = glGetUniformLocation(activeProgram, "uIntensity");
        // glUniform1f(intensityLoc, 0.8f);
    }

    static const float vertexData[] = { -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,  1.0f, 1.0f,  1.0f };
    static const float textureData[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };

    int posLoc = 0;
    int coordLoc = 1;

    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(posLoc, 2, GL_FLOAT, false, 0, vertexData);

    glEnableVertexAttribArray(coordLoc);
    glVertexAttribPointer(coordLoc, 2, GL_FLOAT, false, 0, textureData);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(posLoc);
    glDisableVertexAttribArray(coordLoc);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_renderer->releaseFBO(fbo);
}

} // namespace core
} // namespace video_sdk
