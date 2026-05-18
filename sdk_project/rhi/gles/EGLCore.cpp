#include "EGLCore.h"
#include <iostream>
#include <stdexcept>

namespace video_sdk {
namespace rhi {

// 定义内部使用的特性 Flag
const int FLAG_RECORDABLE = 0x01;  // 告诉 EGL 这个 Surface 可能会被 MediaCodec 用于录制
const int FLAG_TRY_GLES3  = 0x02;  // 尝试创建 GLES 3.x 上下文

EGLCore::EGLCore(EGLContext sharedContext, int flags) {
    if (!initEGL(sharedContext, flags)) {
        throw std::runtime_error("Failed to initialize EGLCore");
    }
}

EGLCore::~EGLCore() {
    if (m_eglDisplay != EGL_NO_DISPLAY) {
        // 解除当前线程的绑定
        eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (m_eglContext != EGL_NO_CONTEXT) {
            eglDestroyContext(m_eglDisplay, m_eglContext);
        }
        eglTerminate(m_eglDisplay);
    }
    m_eglDisplay = EGL_NO_DISPLAY;
    m_eglContext = EGL_NO_CONTEXT;
    m_eglConfig = nullptr;
}

bool EGLCore::initEGL(EGLContext sharedContext, int flags) {
    if (m_eglDisplay != EGL_NO_DISPLAY) return true; // 已初始化

    m_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (m_eglDisplay == EGL_NO_DISPLAY) {
        std::cerr << "eglGetDisplay failed!" << std::endl;
        return false;
    }

    if (!eglInitialize(m_eglDisplay, nullptr, nullptr)) {
        std::cerr << "eglInitialize failed!" << std::endl;
        return false;
    }

    // 1. 配置 EGL 属性 (8888 像素格式, 16位深度)
    int renderableType = EGL_OPENGL_ES2_BIT;
    if ((flags & FLAG_TRY_GLES3) != 0) {
        renderableType |= EGL_OPENGL_ES3_BIT_KHR;
    }

    int attribList[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 16,
        EGL_RENDERABLE_TYPE, renderableType,
        // 这里可以配置 EGL_RECORDABLE_ANDROID 用于硬编兼容
        EGL_NONE
    };

    int numConfigs = 0;
    if (!eglChooseConfig(m_eglDisplay, attribList, &m_eglConfig, 1, &numConfigs) || numConfigs == 0) {
        std::cerr << "eglChooseConfig failed!" << std::endl;
        return false;
    }

    // 2. 创建上下文，最关键的一步是传入 sharedContext
    // 工业级 SDK 中的多线程渲染(UI、采集、编码、解码)必须依赖这个 share 机制
    int contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3, // 强行要求 GLES 3
        EGL_NONE
    };

    // 尝试以 GLES3 创建
    m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, sharedContext, contextAttribs);
    if (eglGetError() != 0x3000 /*EGL_SUCCESS*/ || m_eglContext == EGL_NO_CONTEXT) {
        // Fallback 到 GLES 2
        contextAttribs[1] = 2;
        m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, sharedContext, contextAttribs);
    }

    if (m_eglContext == EGL_NO_CONTEXT) {
        std::cerr << "eglCreateContext failed!" << std::endl;
        return false;
    }

    return true;
}

EGLSurface EGLCore::createWindowSurface(void* nativeWindow) {
    if (!nativeWindow) return EGL_NO_SURFACE;

    int surfaceAttribs[] = {EGL_NONE};
    EGLSurface surface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, nativeWindow, surfaceAttribs);
    if (surface == EGL_NO_SURFACE) {
        std::cerr << "eglCreateWindowSurface failed!" << std::endl;
    }
    return surface;
}

EGLSurface EGLCore::createOffscreenSurface(int width, int height) {
    int surfaceAttribs[] = {
        EGL_WIDTH, width,
        EGL_HEIGHT, height,
        EGL_NONE
    };
    EGLSurface surface = eglCreatePbufferSurface(m_eglDisplay, m_eglConfig, surfaceAttribs);
    if (surface == EGL_NO_SURFACE) {
        std::cerr << "eglCreatePbufferSurface failed!" << std::endl;
    }
    return surface;
}

bool EGLCore::makeCurrent(EGLSurface eglSurface) {
    if (m_eglDisplay == EGL_NO_DISPLAY) return false;

    if (!eglMakeCurrent(m_eglDisplay, eglSurface, eglSurface, m_eglContext)) {
        std::cerr << "eglMakeCurrent failed!" << std::endl;
        return false;
    }
    return true;
}

bool EGLCore::makeCurrentOffscreen() {
    // 创建一个极小的 Dummy PBuffer，用于没有 Window 的纯后台计算线程
    EGLSurface dummy = createOffscreenSurface(1, 1);
    bool ret = makeCurrent(dummy);
    destroySurface(dummy); // makeCurrent 后可以销毁
    return ret;
}

bool EGLCore::swapBuffers(EGLSurface eglSurface) {
    if (m_eglDisplay == EGL_NO_DISPLAY || eglSurface == EGL_NO_SURFACE) return false;
    return eglSwapBuffers(m_eglDisplay, eglSurface);
}

void EGLCore::destroySurface(EGLSurface eglSurface) {
    if (m_eglDisplay != EGL_NO_DISPLAY && eglSurface != EGL_NO_SURFACE) {
        eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(m_eglDisplay, eglSurface);
    }
}

} // namespace rhi
} // namespace video_sdk
