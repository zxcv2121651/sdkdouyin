#include "EGLWindow.h"
#include <iostream>

namespace video_sdk {
namespace rhi {

EGLWindow::EGLWindow(EGLCore* eglCore, void* nativeWindow)
    : m_eglCore(eglCore) {
    if (m_eglCore && nativeWindow) {
        m_eglSurface = m_eglCore->createWindowSurface(nativeWindow);
        // 获取宽高
#ifdef ANDROID
        eglQuerySurface(eglGetCurrentDisplay(), m_eglSurface, EGL_WIDTH, &m_width);
        eglQuerySurface(eglGetCurrentDisplay(), m_eglSurface, EGL_HEIGHT, &m_height);
#endif
    }
}

EGLWindow::~EGLWindow() {
    release();
}

void EGLWindow::makeCurrent() {
    if (m_eglCore && m_eglSurface != EGL_NO_SURFACE) {
        m_eglCore->makeCurrent(m_eglSurface);
    }
}

void EGLWindow::swapBuffers() {
    if (m_eglCore && m_eglSurface != EGL_NO_SURFACE) {
        m_eglCore->swapBuffers(m_eglSurface);
    }
}

void EGLWindow::release() {
    if (m_eglCore && m_eglSurface != EGL_NO_SURFACE) {
        m_eglCore->destroySurface(m_eglSurface);
        m_eglSurface = EGL_NO_SURFACE;
    }
}

} // namespace rhi
} // namespace video_sdk
