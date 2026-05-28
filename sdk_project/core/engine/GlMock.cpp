#include <cstdint>
void glUseProgram(uint32_t program) {}
int glGetUniformLocation(uint32_t program, const char* name) { return 0; }
void glUniformMatrix4fv(int location, int count, bool transpose, const float* value) {}
void glUniform1i(int location, int v0) {}
void glActiveTexture(uint32_t texture) {}
void glBindTexture(uint32_t target, uint32_t texture) {}
void glViewport(int x, int y, int width, int height) {}
void glDrawArrays(uint32_t mode, int first, int count) {}
void glBindFramebuffer(uint32_t target, uint32_t framebuffer) {}
void glEnableVertexAttribArray(uint32_t index) {}
void glVertexAttribPointer(uint32_t index, int size, uint32_t type, bool normalized, int stride, const void* pointer) {}
void glDisableVertexAttribArray(uint32_t index) {}
int glGetAttribLocation(uint32_t program, const char* name) { return 1; }
void glClearColor(float red, float green, float blue, float alpha) {}
void glClear(uint32_t mask) {}
void glBindImageTexture(uint32_t unit, uint32_t texture, int level, bool layered, int layer, uint32_t access, uint32_t format) {}
void glMemoryBarrier(uint32_t barriers) {}
