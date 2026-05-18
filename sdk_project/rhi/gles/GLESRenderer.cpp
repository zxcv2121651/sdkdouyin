#include "GLESRenderer.h"
#include <iostream>

namespace video_sdk {
namespace rhi {

GLESRenderer::GLESRenderer() {}
GLESRenderer::~GLESRenderer() {
    destroy();
}

void GLESRenderer::initialize() {
    // 1. 初始化 GLES 上下文 (通常由外部 EGL 提供环境，这里假设环境已就绪)

    // 2. 动态探测 GPU 能力与版本
    detectCapability();
}

void GLESRenderer::detectCapability() {
    // 模拟从 GL 环境获取版本字符串的逻辑
    // const char* versionStr = (const char*)glGetString(GL_VERSION);
    // 这里使用硬编码模拟解析过程
    std::string versionStr = "OpenGL ES 3.1 V@104.0 (GIT@5df...)";

    if (versionStr.find("OpenGL ES 3.2") != std::string::npos) {
        m_currentVersion = GLESVersion::GLES_3_2;
    } else if (versionStr.find("OpenGL ES 3.1") != std::string::npos) {
        m_currentVersion = GLESVersion::GLES_3_1;
    } else if (versionStr.find("OpenGL ES 3.0") != std::string::npos) {
        m_currentVersion = GLESVersion::GLES_3_0;
    } else if (versionStr.find("OpenGL ES 2.0") != std::string::npos) {
        m_currentVersion = GLESVersion::GLES_2_0;
    } else {
        // 默认回退到 GLES 2.0 以保证基础兼容性
        m_currentVersion = GLESVersion::GLES_2_0;
    }
}

void GLESRenderer::destroy() {
    // 清理 FBO 缓存池和 Shader
}

uint32_t GLESRenderer::acquireFBO(int width, int height) {
    // 实际应根据 FBOKey 从 m_fboPool 获取
    return 0;
}

void GLESRenderer::releaseFBO(uint32_t fboId) {
    // 释放并归还至 m_fboPool
}

std::string GLESRenderer::injectShaderMacros(const std::string& source, bool isVertexShader) {
    std::string finalSource = "";

    // 根据动态探测到的版本，注入对应的 #version 宏
    if (m_currentVersion >= GLESVersion::GLES_3_0) {
        finalSource += "#version 300 es\n";

        // 针对 3.0+ 的特殊宏定义
        finalSource += "#define VARYING in\n";
        finalSource += "#define VARYING_OUT out\n";
        finalSource += "#define TEXTURE texture\n";
    } else {
        finalSource += "#version 100\n";

        // 针对 2.0 的回退宏定义
        finalSource += "#define VARYING varying\n";
        if (isVertexShader) {
            finalSource += "#define VARYING_OUT varying\n";
        } else {
            finalSource += "#define VARYING_OUT \n"; // 2.0 FRAG 中通常使用 gl_FragColor
        }
        finalSource += "#define TEXTURE texture2D\n";
    }

    // 追加精度声明
    if (!isVertexShader) {
        finalSource += "precision highp float;\n";
    }

    // 追加原始 shader 代码
    finalSource += source;

    return finalSource;
}

uint32_t GLESRenderer::compileShader(const std::string& vertexSource, const std::string& fragmentSource) {
    std::string injectedVert = injectShaderMacros(vertexSource, true);
    std::string injectedFrag = injectShaderMacros(fragmentSource, false);

    // 实际的 glCompileShader, glAttachShader 逻辑占位
    // std::cout << "Compiling Vertex Shader:\n" << injectedVert << "\n";
    // std::cout << "Compiling Fragment Shader:\n" << injectedFrag << "\n";

    return 0;
}

} // namespace rhi
} // namespace video_sdk
