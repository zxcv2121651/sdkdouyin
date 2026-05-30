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
    // EGL 初始化通常需要 Surface 或 Pbuffer，此处假定在之前或上层已创建好 context
    probeCapabilities();
}

void GLESRenderer::probeCapabilities() {
    std::cout << "[GLESRenderer] Probing OpenGL ES Capabilities..." << std::endl;

    // 从 GL 获取版本字符串
    const char* versionStrRaw = (const char*)glGetString(GL_VERSION);
    if (!versionStrRaw) {
        std::cerr << "[GLESRenderer] Failed to get GL_VERSION string. Ensure context is bound." << std::endl;
        versionStrRaw = "OpenGL ES 2.0 (Mock)";
    }
    std::string versionStr(versionStrRaw);
    std::cout << "[GLESRenderer] GL_VERSION: " << versionStr << std::endl;

    // 解析 GLES 版本以填充 Capabilities
    if (versionStr.find("OpenGL ES 3.2") != std::string::npos) {
        m_caps.supportsComputeShader = true;
        m_caps.supportsImageLoadStore = true;
        m_caps.supportsPixelBufferObject = true;
    } else if (versionStr.find("OpenGL ES 3.1") != std::string::npos) {
        m_caps.supportsComputeShader = true;
        m_caps.supportsImageLoadStore = true;
        m_caps.supportsPixelBufferObject = true;
    } else if (versionStr.find("OpenGL ES 3.0") != std::string::npos) {
        m_caps.supportsComputeShader = false;
        m_caps.supportsImageLoadStore = false;
        m_caps.supportsPixelBufferObject = true;
    } else {
        m_caps.supportsComputeShader = false;
        m_caps.supportsImageLoadStore = false;
        m_caps.supportsPixelBufferObject = false;
    }

    // 查询最大纹理尺寸
    int maxTexSize = 2048;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);
    m_caps.maxTextureSize = maxTexSize;

    std::cout << "[GLESRenderer] Capabilities => Compute: " << m_caps.supportsComputeShader
              << ", MaxTex: " << m_caps.maxTextureSize << std::endl;
}

void GLESRenderer::destroy() {
    std::lock_guard<std::mutex> lock(m_fboMutex);
    for (auto& pair : m_fboPool) {
        for (auto fbo : pair.second) {
            glDeleteFramebuffers(1, &fbo->fboId);
            glDeleteTextures(1, &fbo->textureId);
            delete fbo;
        }
    }
    m_fboPool.clear();
}

FrameBufferObject* GLESRenderer::acquireFBO(int width, int height) {
    std::lock_guard<std::mutex> lock(m_fboMutex);
    FBOKey key{width, height};

    auto it = m_fboPool.find(key);
    if (it != m_fboPool.end() && !it->second.empty()) {
        auto fbo = it->second.back();
        it->second.pop_back();
        return fbo;
    }

    auto fbo = new FrameBufferObject{0, 0, width, height};
    glGenTextures(1, &fbo->textureId);
    glBindTexture(GL_TEXTURE_2D, fbo->textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenFramebuffers(1, &fbo->fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->fboId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo->textureId, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return fbo;
}

void GLESRenderer::releaseFBO(FrameBufferObject* fbo) {
    if (!fbo) return;
    std::lock_guard<std::mutex> lock(m_fboMutex);
    FBOKey key{fbo->width, fbo->height};
    m_fboPool[key].push_back(fbo);
}

uint32_t GLESRenderer::compileShader(const std::string& vertexSource, const std::string& fragmentSource) {
    return glCreateProgram();
}

uint32_t GLESRenderer::compileComputeShader(const std::string& computeSource) {
    if (!m_caps.supportsComputeShader) {
        std::cerr << "[GLESRenderer] Compute Shader is not supported on this device!" << std::endl;
        return 0;
    }
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
    if (!m_caps.supportsComputeShader) return;
    glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
}

} // namespace rhi
} // namespace video_sdk
