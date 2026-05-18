#include "FilterEngine.h"
#include <iostream>

namespace video_sdk {
namespace core {

FilterEngine::FilterEngine(std::shared_ptr<rhi::IRenderer> renderer) : m_renderer(renderer) {
    initShaders();
}

FilterEngine::~FilterEngine() {
    // Release shader programs
}

void FilterEngine::initShaders() {
    // 一段基础的双边滤波 (Bilateral Filter) Compute Shader，用于美颜磨皮
    // GLES 3.1+ 特性: 使用 imageLoad 和 imageStore 操作
    const std::string beautyComputeSrc = R"(
        layout(local_size_x = 16, local_size_y = 16) in;
        layout(rgba8, binding = 0) readonly uniform highp image2D inputImage;
        layout(rgba8, binding = 1) writeonly uniform highp image2D outputImage;

        uniform float u_intensity; // 磨皮强度

        void main() {
            ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

            // --- 模拟的双边滤波磨皮逻辑 ---
            // 真实情况中这里是带有空间权重(Spatial)和色彩权重(Range)的高斯积分循环
            vec4 centerColor = imageLoad(inputImage, texelCoord);

            // 简单的平滑模糊占位逻辑: 读取周围几个像素平均并基于色差做插值
            vec4 sum = centerColor;
            int count = 1;

            for(int i=-1; i<=1; i++) {
                for(int j=-1; j<=1; j++) {
                    if(i==0 && j==0) continue;
                    vec4 sampleColor = imageLoad(inputImage, texelCoord + ivec2(i, j));
                    float colorDist = distance(centerColor.rgb, sampleColor.rgb);
                    // 只有颜色接近的才混合（保留边缘）
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

    // 假设渲染器是 GLES 3.1+ 且支持 Compute Shader
    m_beautyComputeProgram = m_renderer->compileComputeShader(beautyComputeSrc);
}

void FilterEngine::applyFilter(const std::string& filterName, uint32_t inputTextureId, uint32_t outputTextureId, int width, int height) {
    // 常规渲染管线 FBO 离屏渲染逻辑
    auto fbo = m_renderer->acquireFBO(width, height);

    // ... glBindFramebuffer(GL_FRAMEBUFFER, fbo->fboId);
    // ... glViewport(0, 0, width, height);
    // ... 绘制 Fullscreen Quad

    m_renderer->releaseFBO(fbo);
}

void FilterEngine::applyBeautyCompute(uint32_t inputTextureId, uint32_t outputTextureId, int width, int height, float intensity) {
    if (m_beautyComputeProgram == 0) return;

    // 1. 绑定输入和输出的纹理到 Compute Shader 的 Image Unit 上
    // 对应 Shader 里的 binding = 0 和 binding = 1
    // mock: glBindImageTexture(0, inputTextureId, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
    // mock: glBindImageTexture(1, outputTextureId, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

    // 2. 根据图像宽高和 workgroup size (16x16) 计算需要的组数
    int groupX = (width + 15) / 16;
    int groupY = (height + 15) / 16;

    // 3. 派发并行计算
    m_renderer->dispatchCompute(m_beautyComputeProgram, groupX, groupY, 1);

    // 4. Memory Barrier 确保计算完成并且结果写入显存
    // mock: glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

} // namespace core
} // namespace video_sdk
