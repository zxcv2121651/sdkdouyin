#include "FilterEngine.h"
#include <iostream>

extern void glUseProgram(uint32_t program);
extern int glGetAttribLocation(uint32_t program, const char* name);
extern int glGetUniformLocation(uint32_t program, const char* name);
extern void glUniform1i(int location, int v0);
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
    const std::string beautyComputeSrc = R"(
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

    m_beautyComputeProgram = m_renderer->compileComputeShader(beautyComputeSrc);
}

void FilterEngine::applyFilter(const std::string& filterName, uint32_t inputTextureId, uint32_t outputTextureId, int width, int height) {
    // 工业级常规管线离屏渲染：向 FBO 池借用一个 FBO
    auto fbo = m_renderer->acquireFBO(width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo->fboId);
    glViewport(0, 0, width, height);

    // 这里以假定的一个 programId 演示
    uint32_t mockProgramId = 1;
    glUseProgram(mockProgramId);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTextureId);
    // int texLoc = glGetUniformLocation(mockProgramId, "uTexture");
    // glUniform1i(texLoc, 0);

    static const float vertexData[] = { -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,  1.0f, 1.0f,  1.0f };
    static const float textureData[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };

    int posLoc = 0; // glGetAttribLocation
    int coordLoc = 1; // glGetAttribLocation

    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(posLoc, 2, GL_FLOAT, false, 0, vertexData);

    glEnableVertexAttribArray(coordLoc);
    glVertexAttribPointer(coordLoc, 2, GL_FLOAT, false, 0, textureData);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(posLoc);
    glDisableVertexAttribArray(coordLoc);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 如果上层需要获取最终的 Texture，可以在 fbo->textureId 中获取
    // 渲染完毕，将其归还回资源池
    m_renderer->releaseFBO(fbo);
}

void FilterEngine::applyBeautyCompute(uint32_t inputTextureId, uint32_t outputTextureId, int width, int height, float intensity) {
    if (m_beautyComputeProgram == 0) return;

    glUseProgram(m_beautyComputeProgram);

    // 绑定输入和输出的纹理到 Compute Shader 的 Image Unit 上
    glBindImageTexture(0, inputTextureId, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
    glBindImageTexture(1, outputTextureId, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

    // 传入 Uniform
    // int intensityLoc = glGetUniformLocation(m_beautyComputeProgram, "u_intensity");
    // glUniform1f(intensityLoc, intensity);

    // 根据图像宽高和 workgroup size (16x16) 计算需要的组数
    int groupX = (width + 15) / 16;
    int groupY = (height + 15) / 16;

    // 派发并行计算
    m_renderer->dispatchCompute(m_beautyComputeProgram, groupX, groupY, 1);

    // Memory Barrier 确保计算完成并且结果写入显存，以便管线下一阶段能读到
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

} // namespace core
} // namespace video_sdk
