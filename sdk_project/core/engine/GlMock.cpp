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
