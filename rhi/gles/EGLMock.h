#pragma once
#include <cstdint>

// 模拟 EGL 类型
typedef void* EGLDisplay;
typedef void* EGLSurface;
typedef void* EGLContext;
typedef void* EGLConfig;
typedef int32_t EGLint;
typedef int32_t EGLBoolean;

#define EGL_NO_DISPLAY ((EGLDisplay)0)
#define EGL_NO_SURFACE ((EGLSurface)0)
#define EGL_NO_CONTEXT ((EGLContext)0)
#define EGL_DEFAULT_DISPLAY ((void*)0)

#define EGL_TRUE 1
#define EGL_FALSE 0

#define EGL_RED_SIZE 0x3024
#define EGL_GREEN_SIZE 0x3023
#define EGL_BLUE_SIZE 0x3022
#define EGL_ALPHA_SIZE 0x3021
#define EGL_DEPTH_SIZE 0x3025
#define EGL_STENCIL_SIZE 0x3026
#define EGL_RENDERABLE_TYPE 0x3040
#define EGL_NONE 0x3038
#define EGL_OPENGL_ES2_BIT 0x0004
#define EGL_OPENGL_ES3_BIT_KHR 0x0040
#define EGL_SURFACE_TYPE 0x3033
#define EGL_WINDOW_BIT 0x0004
#define EGL_PBUFFER_BIT 0x0001
#define EGL_CONTEXT_CLIENT_VERSION 0x3098
#define EGL_WIDTH 0x3057
#define EGL_HEIGHT 0x3056

// 模拟 EGL 方法
inline EGLDisplay eglGetDisplay(void* display_id) { return (EGLDisplay)1; }
inline EGLBoolean eglInitialize(EGLDisplay dpy, EGLint *major, EGLint *minor) { return EGL_TRUE; }
inline EGLBoolean eglChooseConfig(EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config) { *num_config = 1; return EGL_TRUE; }
inline EGLContext eglCreateContext(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list) { return (EGLContext)2; }
inline EGLSurface eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config, void* win, const EGLint *attrib_list) { return (EGLSurface)3; }
inline EGLSurface eglCreatePbufferSurface(EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list) { return (EGLSurface)4; }
inline EGLBoolean eglMakeCurrent(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx) { return EGL_TRUE; }
inline EGLBoolean eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) { return EGL_TRUE; }
inline EGLBoolean eglDestroySurface(EGLDisplay dpy, EGLSurface surface) { return EGL_TRUE; }
inline EGLBoolean eglDestroyContext(EGLDisplay dpy, EGLContext ctx) { return EGL_TRUE; }
inline EGLBoolean eglTerminate(EGLDisplay dpy) { return EGL_TRUE; }
inline EGLContext eglGetCurrentContext() { return EGL_NO_CONTEXT; }
inline EGLint eglGetError() { return 0x3000; } // EGL_SUCCESS
