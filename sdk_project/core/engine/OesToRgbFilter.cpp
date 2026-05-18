#include "OesToRgbFilter.h"
#include <iostream>

// 模拟 OpenGL 的 glUniform 等函数
// 实际运行需要 include GLES2/gl2.h
extern void glUseProgram(uint32_t program);
extern int glGetUniformLocation(uint32_t program, const char* name);
extern void glUniformMatrix4fv(int location, int count, bool transpose, const float* value);
extern void glUniform1i(int location, int v0);
extern void glActiveTexture(uint32_t texture);
extern void glBindTexture(uint32_t target, uint32_t texture);
extern void glViewport(int x, int y, int width, int height);
extern void glDrawArrays(uint32_t mode, int first, int count);
extern void glBindFramebuffer(uint32_t target, uint32_t framebuffer);
#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE_EXTERNAL_OES 0x8D65
#define GL_FRAMEBUFFER 0x8D40
#define GL_TRIANGLE_STRIP 0x0005

namespace video_sdk {
namespace core {

OesToRgbFilter::OesToRgbFilter(std::shared_ptr<rhi::IRenderer> renderer) : m_renderer(renderer) {
    initShader();
}

OesToRgbFilter::~OesToRgbFilter() {
    // 释放 shader
}

void OesToRgbFilter::initShader() {
    const std::string vertexShader = R"(
        attribute vec4 aPosition;
        attribute vec4 aTextureCoord;
        uniform mat4 uTextureMatrix;
        varying vec2 vTextureCoord;
        void main() {
            gl_Position = aPosition;
            // OES 纹理的核心: 纹理坐标必须先乘以硬件矩阵
            vTextureCoord = (uTextureMatrix * aTextureCoord).xy;
        }
    )";

    // 关键指令: 引入 OES 扩展宏，并使用 samplerExternalOES 进行采样
    const std::string fragmentShader = R"(
        #extension GL_OES_EGL_image_external : require
        precision mediump float;
        varying vec2 vTextureCoord;
        uniform samplerExternalOES uTexture;
        void main() {
            gl_FragColor = texture2D(uTexture, vTextureCoord);
        }
    )";

    // 在真实的 SDK 中，这里会调用 m_renderer->compileShader
    m_programId = m_renderer->compileShader(vertexShader, fragmentShader);

    // 模拟获取 Location
    // m_matrixLoc = glGetUniformLocation(m_programId, "uTextureMatrix");
    // m_textureLoc = glGetUniformLocation(m_programId, "uTexture");
    m_matrixLoc = 0;
    m_textureLoc = 1;
}

void OesToRgbFilter::render(uint32_t oesTextureId, rhi::FrameBufferObject* fbo, const float* matrix) {
    if (!fbo || m_programId == 0) return;

    // 1. 绑定目标 FBO (离屏渲染目标)
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->fboId);
    glViewport(0, 0, fbo->width, fbo->height);

    // 2. 激活 Shader
    glUseProgram(m_programId);

    // 3. 传入从 Android Camera/MediaCodec 读出的仿射变换矩阵
    glUniformMatrix4fv(m_matrixLoc, 1, false, matrix);

    // 4. 绑定外部纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, oesTextureId);
    glUniform1i(m_textureLoc, 0);

    // 5. 设置顶点坐标、纹理坐标(代码省略)...

    // 6. 执行全屏矩形绘制 (Zero-Copy OES 转码过程完成)
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // 7. 解绑 FBO
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);
}

} // namespace core
} // namespace video_sdk
