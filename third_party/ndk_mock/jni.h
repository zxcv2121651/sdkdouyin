#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* JNIEnv;
typedef void* jobject;
typedef void* jclass;
typedef void* jstring;
typedef void* jfloatArray;
typedef int64_t jlong;
typedef int32_t jint;
typedef uint8_t jboolean;

#define JNIEXPORT
#define JNICALL
#define JNI_FALSE 0
#define JNI_TRUE 1

// Dummy struct for mocked env to let GetFloatArrayRegion compile
struct _JNIEnv {
    void GetFloatArrayRegion(jfloatArray array, jint start, jint len, float* buf) {}
};

#ifdef __cplusplus
}
#endif
