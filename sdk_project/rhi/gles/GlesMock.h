#pragma once
// 这是一个宏定义头文件，用于在非实际运行环境(如编译沙盒)模拟 OpenGL ES 的宏和函数，确保代码编译通过。
// 实际工程中应包含 <GLES3/gl31.h> 等头文件。
#include <cstdint>

#define GL_VERSION 0x1F02
#define GL_TEXTURE_2D 0x0DE1
#define GL_RGBA 0x1908
#define GL_UNSIGNED_BYTE 0x1401
#define GL_FRAMEBUFFER 0x8D40
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_LINEAR 0x2601
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_COMPUTE_SHADER 0x91B9

inline const char* glGetString(uint32_t name) { return "OpenGL ES 3.1"; }
inline void glGenTextures(int n, uint32_t* textures) { *textures = 1; }
inline void glBindTexture(uint32_t target, uint32_t texture) {}
inline void glTexImage2D(uint32_t target, int level, int internalformat, int width, int height, int border, uint32_t format, uint32_t type, const void* pixels) {}
inline void glTexParameteri(uint32_t target, uint32_t pname, int param) {}
inline void glGenFramebuffers(int n, uint32_t* framebuffers) { *framebuffers = 1; }
inline void glBindFramebuffer(uint32_t target, uint32_t framebuffer) {}
inline void glFramebufferTexture2D(uint32_t target, uint32_t attachment, uint32_t textarget, uint32_t texture, int level) {}
inline void glDeleteFramebuffers(int n, const uint32_t* framebuffers) {}
inline void glDeleteTextures(int n, const uint32_t* textures) {}
inline uint32_t glCreateShader(uint32_t type) { return 1; }
inline void glShaderSource(uint32_t shader, int count, const char** string, const int* length) {}
inline void glCompileShader(uint32_t shader) {}
inline uint32_t glCreateProgram() { return 1; }
inline void glAttachShader(uint32_t program, uint32_t shader) {}
inline void glLinkProgram(uint32_t program) {}
inline void glDeleteShader(uint32_t shader) {}
inline void glDispatchCompute(uint32_t num_groups_x, uint32_t num_groups_y, uint32_t num_groups_z) {}
inline void glBindImageTexture(uint32_t unit, uint32_t texture, int level, bool layered, int layer, uint32_t access, uint32_t format) {}
