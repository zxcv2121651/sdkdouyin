#include "GLESRenderer.h"
#include "GlesMock.h"
#include <iostream>

namespace video_sdk {
namespace rhi {

GLESRenderer::GLESRenderer() {}

GLESRenderer::~GLESRenderer() {
    destroy();
}

void GLESRenderer::initialize() {
    detectCapability();
}

void GLESRenderer::detectCapability() {
    std::string versionStr = (const char*)glGetString(GL_VERSION);

    if (versionStr.find("OpenGL ES 3.2") != std::string::npos) {
        m_currentVersion = GLESVersion::GLES_3_2;
    } else if (versionStr.find("OpenGL ES 3.1") != std::string::npos) {
        m_currentVersion = GLESVersion::GLES_3_1;
    } else if (versionStr.find("OpenGL ES 3.0") != std::string::npos) {
        m_currentVersion = GLESVersion::GLES_3_0;
    } else {
        m_currentVersion = GLESVersion::GLES_2_0;
    }
}

void GLESRenderer::destroy() {
    std::lock_guard<std::mutex> lock(m_fboMutex);
    for (auto& pair : m_fboPool) {
        for (auto& fbo : pair.second) {
            glDeleteFramebuffers(1, &fbo->fboId);
            glDeleteTextures(1, &fbo->textureId);
        }
    }
    m_fboPool.clear();
}

FrameBufferObject* GLESRenderer::createFBOInternal(int width, int height) {
    auto fbo = new FrameBufferObject{0, 0, width, height};

    // 1. 生成空纹理
    glGenTextures(1, &fbo->textureId);
    glBindTexture(GL_TEXTURE_2D, fbo->textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // 纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // 2. 生成 FBO 并绑定纹理
    glGenFramebuffers(1, &fbo->fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->fboId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo->textureId, 0);

    // 恢复默认 Framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return fbo;
}

FrameBufferObject* GLESRenderer::acquireFBO(int width, int height) {
    std::lock_guard<std::mutex> lock(m_fboMutex);
    FBOKey key{width, height};

    auto it = m_fboPool.find(key);
    if (it != m_fboPool.end() && !it->second.empty()) {
        // 从池子尾部弹出一个可用的 FBO
        auto fbo = std::move(it->second.back());
        it->second.pop_back();
        return fbo.release();
    }

    // 如果池子里没有，进行真正的 GPU 资源分配
    return createFBOInternal(width, height);
}

void GLESRenderer::releaseFBO(FrameBufferObject* fbo) {
    if (!fbo) return;

    std::lock_guard<std::mutex> lock(m_fboMutex);
    FBOKey key{fbo->width, fbo->height};

    // 将其重新归还至池中
    m_fboPool[key].push_back(std::unique_ptr<FrameBufferObject>(fbo));
}

std::string GLESRenderer::injectShaderMacros(const std::string& source, bool isVertexShader) {
    // 简化处理用于演示
    return "#version 300 es\n" + source;
}

uint32_t GLESRenderer::compileShader(const std::string& vertexSource, const std::string& fragmentSource) {
    // 编译常规渲染管线 Shader
    return glCreateProgram();
}

uint32_t GLESRenderer::compileComputeShader(const std::string& computeSource) {
    // GLES 3.1+ Compute Shader 编译
    std::string injected = "#version 310 es\n" + computeSource;
    uint32_t shader = glCreateShader(GL_COMPUTE_SHADER);
    const char* src = injected.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    uint32_t program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);
    glDeleteShader(shader);
    return program;
}

void GLESRenderer::dispatchCompute(uint32_t programId, int numGroupsX, int numGroupsY, int numGroupsZ) {
    // 触发 GLES Compute Shader 计算
    glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
}

} // namespace rhi
} // namespace video_sdk
