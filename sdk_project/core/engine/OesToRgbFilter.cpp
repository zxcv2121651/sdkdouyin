#include "OesToRgbFilter.h"
#include <iostream>

extern void glUseProgram(uint32_t program);
extern int glGetUniformLocation(uint32_t program, const char* name);
extern int glGetAttribLocation(uint32_t program, const char* name);
extern void glUniformMatrix4fv(int location, int count, bool transpose, const float* value);
extern void glUniform1i(int location, int v0);
extern void glActiveTexture(uint32_t texture);
extern void glBindTexture(uint32_t target, uint32_t texture);
extern void glViewport(int x, int y, int width, int height);
extern void glDrawArrays(uint32_t mode, int first, int count);
extern void glBindFramebuffer(uint32_t target, uint32_t framebuffer);
extern void glEnableVertexAttribArray(uint32_t index);
extern void glDisableVertexAttribArray(uint32_t index);
extern void glVertexAttribPointer(uint32_t index, int size, uint32_t type, bool normalized, int stride, const void* pointer);
extern void glClearColor(float red, float green, float blue, float alpha);
extern void glClear(uint32_t mask);

#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE_EXTERNAL_OES 0x8D65
#define GL_FRAMEBUFFER 0x8D40
#define GL_TRIANGLE_STRIP 0x0005
#define GL_FLOAT 0x1406
#define GL_COLOR_BUFFER_BIT 0x00004000

namespace video_sdk {
namespace core {

OesToRgbFilter::OesToRgbFilter(std::shared_ptr<rhi::IRenderer> renderer) : m_renderer(renderer) {
    initShader();
}

OesToRgbFilter::~OesToRgbFilter() {
    // 调用 renderer 的接口释放 programId
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

    m_programId = m_renderer->compileShader(vertexShader, fragmentShader);

    // 真实的 Location 获取
    m_matrixLoc = glGetUniformLocation(m_programId, "uTextureMatrix");
    m_textureLoc = glGetUniformLocation(m_programId, "uTexture");
    m_posLoc = glGetAttribLocation(m_programId, "aPosition");
    m_coordLoc = glGetAttribLocation(m_programId, "aTextureCoord");
}

void OesToRgbFilter::render(uint32_t oesTextureId, rhi::FrameBufferObject* fbo, const float* matrix) {
    if (!fbo || m_programId == 0) return;

    // 1. 绑定目标 FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->fboId);
    glViewport(0, 0, fbo->width, fbo->height);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_programId);

    // 2. 传入相机矩阵
    glUniformMatrix4fv(m_matrixLoc, 1, false, matrix);

    // 3. 绑定外部纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, oesTextureId);
    glUniform1i(m_textureLoc, 0);

    // 4. 顶点数据 (VBO 坐标) - 填满真实绘制指令
    // 标准的 OpenGL NDC 坐标
    static const float vertexData[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f
    };

    // 标准的纹理坐标 (Android Camera2 吐出的需要修正)
    static const float textureData[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f
    };

    glEnableVertexAttribArray(m_posLoc);
    glVertexAttribPointer(m_posLoc, 2, GL_FLOAT, false, 0, vertexData);

    glEnableVertexAttribArray(m_coordLoc);
    glVertexAttribPointer(m_coordLoc, 2, GL_FLOAT, false, 0, textureData);

    // 5. 执行绘制
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // 6. 收尾清理
    glDisableVertexAttribArray(m_posLoc);
    glDisableVertexAttribArray(m_coordLoc);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace core
} // namespace video_sdk
