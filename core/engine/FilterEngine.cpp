#include "FilterEngine.h"
#include <iostream>

extern void glUseProgram(uint32_t program);
extern int glGetAttribLocation(uint32_t program, const char* name);
extern int glGetUniformLocation(uint32_t program, const char* name);
extern void glUniform1i(int location, int v0);
extern void glUniform1f(int location, float v0);
extern void glActiveTexture(uint32_t texture);
extern void glBindTexture(uint32_t target, uint32_t texture);
extern void glViewport(int x, int y, int width, int height);
extern void glDrawArrays(uint32_t mode, int first, int count);
extern void glBindFramebuffer(uint32_t target, uint32_t framebuffer);
extern void glEnableVertexAttribArray(uint32_t index);
extern void glDisableVertexAttribArray(uint32_t index);
extern void glVertexAttribPointer(uint32_t index, int size, uint32_t type, bool normalized, int stride, const void* pointer);
extern void glBindImageTexture(uint32_t unit, uint32_t texture, int level, bool layered, int layer, uint32_t access, uint32_t format);
extern void glMemoryBarrier(uint32_t barriers);

#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE_2D 0x0DE1
#define GL_FRAMEBUFFER 0x8D40
#define GL_TRIANGLE_STRIP 0x0005
#define GL_FLOAT 0x1406
#define GL_FALSE 0
#define GL_READ_ONLY 0x88B8
#define GL_WRITE_ONLY 0x88B9
#define GL_RGBA8 0x8058
#define GL_SHADER_IMAGE_ACCESS_BARRIER_BIT 0x00000020

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

    const std::string beautyComputeSrc = R"(
        #version 310 es
        layout(local_size_x = 16, local_size_y = 16) in;
        layout(rgba8, binding = 0) readonly uniform highp image2D inputImage;
        layout(rgba8, binding = 1) writeonly uniform highp image2D outputImage;

        uniform float u_intensity;

        void main() {
            ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
            vec4 centerColor = imageLoad(inputImage, texelCoord);

            vec4 sum = centerColor;
            int count = 1;

            for(int i=-1; i<=1; i++) {
                for(int j=-1; j<=1; j++) {
                    if(i==0 && j==0) continue;
                    vec4 sampleColor = imageLoad(inputImage, texelCoord + ivec2(i, j));
                    float colorDist = distance(centerColor.rgb, sampleColor.rgb);
                    if(colorDist < 0.2) {
                        sum += sampleColor;
                        count++;
                    }
                }
            }

            vec4 smoothedColor = sum / float(count);
            vec4 finalColor = mix(centerColor, smoothedColor, u_intensity);

            imageStore(outputImage, texelCoord, finalColor);
        }
    )";

    if (m_renderer) {
        m_beautyComputeProgram = m_renderer->compileComputeShader(beautyComputeSrc);
    }
}

void FilterEngine::applyFilter(const std::string& filterName, uint32_t inputTextureId, uint32_t outputTextureId, int width, int height) {
    if (!m_renderer) return;

    auto fbo = m_renderer->acquireFBO(width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo->fboId);
    glViewport(0, 0, width, height);

    uint32_t mockProgramId = m_defaultProgram > 0 ? m_defaultProgram : 1;
    glUseProgram(mockProgramId);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTextureId);

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

void FilterEngine::applyBeautyCompute(uint32_t inputTextureId, uint32_t outputTextureId, int width, int height, float intensity) {
    if (m_beautyComputeProgram == 0 || !m_renderer) return;

    std::cout << "[FilterEngine] Applying Compute Shader Beauty Filter. Intensity: " << intensity << std::endl;

    glUseProgram(m_beautyComputeProgram);

    glBindImageTexture(0, inputTextureId, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
    glBindImageTexture(1, outputTextureId, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

    // Assuming we have the location
    // int intensityLoc = glGetUniformLocation(m_beautyComputeProgram, "u_intensity");
    // glUniform1f(intensityLoc, intensity);

    int groupX = (width + 15) / 16;
    int groupY = (height + 15) / 16;

    m_renderer->dispatchCompute(m_beautyComputeProgram, groupX, groupY, 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

} // namespace core
} // namespace video_sdk
