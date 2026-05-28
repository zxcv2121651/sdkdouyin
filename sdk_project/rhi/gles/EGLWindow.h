#pragma once
#include "EGLCore.h"

namespace video_sdk {
namespace rhi {

/**
 * @brief 封装具体的 Window Surface 渲染目标 (例如 Android SurfaceView 对应的 ANativeWindow)
 */
class EGLWindow {
public:
    EGLWindow(EGLCore* eglCore, void* nativeWindow);
    ~EGLWindow();

    void makeCurrent();
    void swapBuffers();
    void release();

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    EGLCore* m_eglCore = nullptr;
    EGLSurface m_eglSurface = EGL_NO_SURFACE;
    int m_width = 0;
    int m_height = 0;
};

} // namespace rhi
} // namespace video_sdk
