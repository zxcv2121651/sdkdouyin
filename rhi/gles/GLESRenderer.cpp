#include "GLESRenderer.h"
#include <iostream>

// 模拟或真实引入 GLES3 头文件
#ifdef ANDROID
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#else
// 如果是 Mock 环境，声明我们需要用到的 API
extern void glGenTextures(int n, uint32_t* textures);
extern void glBindTexture(uint32_t target, uint32_t texture);
extern void glTexParameteri(uint32_t target, uint32_t pname, int param);
extern void glTexImage2D(uint32_t target, int level, int internalformat, int width, int height, int border, uint32_t format, uint32_t type, const void* pixels);
extern void glDeleteTextures(int n, const uint32_t* textures);
extern void glGenFramebuffers(int n, uint32_t* framebuffers);
extern void glBindFramebuffer(uint32_t target, uint32_t framebuffer);
extern void glFramebufferTexture2D(uint32_t target, uint32_t attachment, uint32_t textarget, uint32_t texture, int level);
extern void glDeleteFramebuffers(int n, const uint32_t* framebuffers);
extern void glViewport(int x, int y, int width, int height);
extern void glClearColor(float red, float green, float blue, float alpha);
extern void glClear(uint32_t mask);
extern uint32_t glCreateShader(uint32_t type);
extern void glShaderSource(uint32_t shader, int count, const char** string, const int* length);
extern void glCompileShader(uint32_t shader);
extern void glGetShaderiv(uint32_t shader, uint32_t pname, int* params);
extern uint32_t glCreateProgram();
extern void glAttachShader(uint32_t program, uint32_t shader);
extern void glLinkProgram(uint32_t program);
extern void glGetProgramiv(uint32_t program, uint32_t pname, int* params);
extern void glDeleteShader(uint32_t shader);
extern void glDeleteProgram(uint32_t program);
extern void glUseProgram(uint32_t program);
extern int glGetUniformLocation(uint32_t program, const char* name);
extern int glGetAttribLocation(uint32_t program, const char* name);
extern void glUniformMatrix4fv(int location, int count, bool transpose, const float* value);
extern void glUniform1i(int location, int v0);
extern void glActiveTexture(uint32_t texture);
extern void glDrawArrays(uint32_t mode, int first, int count);
extern void glEnableVertexAttribArray(uint32_t index);
extern void glDisableVertexAttribArray(uint32_t index);
extern void glVertexAttribPointer(uint32_t index, int size, uint32_t type, bool normalized, int stride, const void* pointer);

#define GL_TEXTURE_2D 0x0DE1
#define GL_TEXTURE_EXTERNAL_OES 0x8D65
#define GL_RGBA 0x1908
#define GL_UNSIGNED_BYTE 0x1401
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_LINEAR 0x2601
#define GL_FRAMEBUFFER 0x8D40
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B32
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_TEXTURE0 0x84C0
#define GL_FLOAT 0x1406
#endif

namespace video_sdk {
namespace rhi {

// ----------- GLESShader -----------
GLESShader::~GLESShader() {
    if (programId > 0) {
        glDeleteProgram(programId);
    }
}

// ----------- GLESTexture -----------
GLESTexture::GLESTexture(int w, int h, TextureFormat fmt) : width(w), height(h), format(fmt) {
    if (fmt == TextureFormat::RGBA8) {
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        // 创建关联的 FBO 用于 RenderTarget
        glGenFramebuffers(1, &fboId);
        glBindFramebuffer(GL_FRAMEBUFFER, fboId);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    } else {
        // External OES texture id is passed in, handled by wrapper
        textureId = 0;
    }
}

GLESTexture::~GLESTexture() {
    if (fboId > 0) {
        glDeleteFramebuffers(1, &fboId);
    }
    if (textureId > 0 && format != TextureFormat::OES_EXTERNAL) { // OES id managed externally
        glDeleteTextures(1, &textureId);
    }
}

// ----------- GLESRenderer -----------
GLESRenderer::GLESRenderer() {
    std::cout << "[GLESRenderer] Initialized." << std::endl;
}

GLESRenderer::~GLESRenderer() {}

std::shared_ptr<ITexture> GLESRenderer::createTexture2D(int width, int height, TextureFormat format) {
    return std::make_shared<GLESTexture>(width, height, format);
}

std::shared_ptr<ITexture> GLESRenderer::wrapExternalOESTexture(uint32_t textureId, int width, int height) {
    auto tex = std::make_shared<GLESTexture>(width, height, TextureFormat::OES_EXTERNAL);
    tex->textureId = textureId; // Set external ID
    return tex;
}

uint32_t GLESRenderer::compileShaderInternal(uint32_t type, const std::string& source) {
    uint32_t shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        std::cerr << "[GLESRenderer] Shader compile failed!" << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

std::shared_ptr<IShader> GLESRenderer::createShader(const std::string& vertexSource, const std::string& fragmentSource) {
    auto shader = std::make_shared<GLESShader>();
    uint32_t vs = compileShaderInternal(GL_VERTEX_SHADER, vertexSource);
    uint32_t fs = compileShaderInternal(GL_FRAGMENT_SHADER, fragmentSource);

    shader->programId = glCreateProgram();
    glAttachShader(shader->programId, vs);
    glAttachShader(shader->programId, fs);
    glLinkProgram(shader->programId);

    int status;
    glGetProgramiv(shader->programId, GL_LINK_STATUS, &status);
    if (!status) {
        std::cerr << "[GLESRenderer] Program link failed!" << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return shader;
}

void GLESRenderer::setRenderTarget(std::shared_ptr<ITexture> texture) {
    if (!texture) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // backbuffer
        return;
    }

    auto glesTex = std::static_pointer_cast<GLESTexture>(texture);
    if (glesTex->fboId > 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, glesTex->fboId);
        glViewport(0, 0, glesTex->width, glesTex->height);
    }
}

void GLESRenderer::clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GLESRenderer::bindShader(std::shared_ptr<IShader> shader) {
    if (!shader) return;
    auto glesShader = std::static_pointer_cast<GLESShader>(shader);
    glUseProgram(glesShader->programId);
}

void GLESRenderer::bindTexture(std::shared_ptr<IShader> shader, const std::string& uniformName, std::shared_ptr<ITexture> texture, int slot) {
    if (!shader || !texture) return;
    auto glesShader = std::static_pointer_cast<GLESShader>(shader);
    auto glesTex = std::static_pointer_cast<GLESTexture>(texture);

    int loc = 0;
    if (glesShader->uniformLocations.find(uniformName) == glesShader->uniformLocations.end()) {
        loc = glGetUniformLocation(glesShader->programId, uniformName.c_str());
        glesShader->uniformLocations[uniformName] = loc;
    } else {
        loc = glesShader->uniformLocations[uniformName];
    }

    glActiveTexture(GL_TEXTURE0 + slot);
    uint32_t target = (glesTex->format == TextureFormat::OES_EXTERNAL) ? GL_TEXTURE_EXTERNAL_OES : GL_TEXTURE_2D;
    glBindTexture(target, glesTex->textureId);
    glUniform1i(loc, slot);
}

void GLESRenderer::setUniformMatrix4fv(std::shared_ptr<IShader> shader, const std::string& name, const float* matrix) {
    if (!shader) return;
    auto glesShader = std::static_pointer_cast<GLESShader>(shader);

    int loc = 0;
    if (glesShader->uniformLocations.find(name) == glesShader->uniformLocations.end()) {
        loc = glGetUniformLocation(glesShader->programId, name.c_str());
        glesShader->uniformLocations[name] = loc;
    } else {
        loc = glesShader->uniformLocations[name];
    }
    glUniformMatrix4fv(loc, 1, false, matrix);
}

void GLESRenderer::drawArrays(int mode, int first, int count, const std::vector<VertexAttribute>& attributes) {
    // 假设当前 shader 已经 bind
    std::vector<int> enabledAttribs;

    // In a real implementation, we need the shader to get attrib locations.
    // Here we assume the user passed the location index directly in `index` for simplicity,
    // or we query it.

    for (const auto& attr : attributes) {
        glEnableVertexAttribArray(attr.index);
        glVertexAttribPointer(attr.index, attr.size, GL_FLOAT, false, attr.stride, attr.pointer);
        enabledAttribs.push_back(attr.index);
    }

    // mode == 5 is GL_TRIANGLE_STRIP
    glDrawArrays(mode, first, count);

    for (int idx : enabledAttribs) {
        glDisableVertexAttribArray(idx);
    }
}

void GLESRenderer::presentToScreen() {
    // 调用 EGL swapBuffers 等
}

} // namespace rhi
} // namespace video_sdk
