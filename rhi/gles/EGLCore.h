#pragma once
#include <cstdint>

#ifdef ANDROID
#include <EGL/egl.h>
#else
#include "EGLMock.h"
#endif

namespace video_sdk {
namespace rhi {

/**
 * @brief 工业级跨线程 EGL 上下文管理器。
 * 解决多线程渲染（如相机采集线程 -> 滤镜处理线程 -> 视频编码线程）之间的纹理资源共享问题。
 */
class EGLCore {
public:
    // 构造时可传入一个外部的 shareContext，以实现多个线程共享纹理池和 Shader
    EGLCore(EGLContext sharedContext = EGL_NO_CONTEXT, int flags = 0);
    ~EGLCore();

    // 根据外部 Window (如 Android 的 Surface) 创建 EGLSurface
    EGLSurface createWindowSurface(void* nativeWindow);

    // 创建一个离屏的 PBuffer Surface (适用于纯后台解码、离屏渲染引擎、导出引擎)
    EGLSurface createOffscreenSurface(int width, int height);

    // 将当前的 Context 与 Surface 绑定到当前调用线程
    bool makeCurrent(EGLSurface eglSurface);

    // 绑定到当前线程，但不指定任何输入输出 Surface (通常在只进行 Compute Shader 或完全使用 FBO 离屏渲染时使用)
    bool makeCurrentOffscreen();

    // 交换缓冲区，将渲染结果输出到屏幕
    bool swapBuffers(EGLSurface eglSurface);

    // 销毁指定的 Surface
    void destroySurface(EGLSurface eglSurface);

    // 获取当前的 Context，以便传给子线程进行 share
    EGLContext getContext() const { return m_eglContext; }

private:
    bool initEGL(EGLContext sharedContext, int flags);

private:
    EGLDisplay m_eglDisplay = EGL_NO_DISPLAY;
    EGLConfig  m_eglConfig = nullptr;
    EGLContext m_eglContext = EGL_NO_CONTEXT;
};

} // namespace rhi
} // namespace video_sdk
