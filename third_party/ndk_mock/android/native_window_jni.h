#pragma once
#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ANativeWindow ANativeWindow;

inline ANativeWindow* ANativeWindow_fromSurface(JNIEnv* env, jobject surface) { return (ANativeWindow*)1; }
inline void ANativeWindow_release(ANativeWindow* window) {}
inline void ANativeWindow_acquire(ANativeWindow* window) {}

#ifdef __cplusplus
}
#endif
