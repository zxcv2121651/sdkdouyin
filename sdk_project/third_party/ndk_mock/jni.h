#pragma once
// Mock jni.h for local compilation test

#ifdef __cplusplus
extern "C" {
#endif

#define JNIEXPORT
#define JNICALL
#define JNI_TRUE 1
#define JNI_FALSE 0

typedef struct _JNIEnv JNIEnv;
typedef void* jobject;
typedef void* jclass;
typedef void* jstring;
typedef long long jlong;
typedef int jint;
typedef float jfloat;
typedef unsigned char jboolean;

struct JNINativeInterface {
    const char* (*GetStringUTFChars)(JNIEnv*, jstring, jboolean*);
    void (*ReleaseStringUTFChars)(JNIEnv*, jstring, const char*);
    void (*ExceptionDescribe)(JNIEnv*);
    void (*ExceptionClear)(JNIEnv*);
    jint (*ThrowNew)(JNIEnv*, jclass, const char*);
    jclass (*FindClass)(JNIEnv*, const char*);
};

struct _JNIEnv {
    const struct JNINativeInterface* functions;

#ifdef __cplusplus
    const char* GetStringUTFChars(jstring str, jboolean* isCopy) { return "mock_string"; }
    void ReleaseStringUTFChars(jstring str, const char* chars) {}
    void ExceptionDescribe() {}
    void ExceptionClear() {}
    jint ThrowNew(jclass clazz, const char* message) { return 0; }
    jclass FindClass(const char* name) { return nullptr; }
#endif
};

#ifdef __cplusplus
}
#endif
