#include <jni.h>
#include <string>
#include <memory>
#include "JniExceptionHandler.h"
#include "modules/camera/CameraCapture.h"

// 引入 Android 的原生 Window 头文件
#ifdef ANDROID
#include <android/native_window_jni.h>
#else
// Mock for test build
typedef void* ANativeWindow;
ANativeWindow* ANativeWindow_fromSurface(JNIEnv* env, jobject surface) { return nullptr; }
void ANativeWindow_release(ANativeWindow* window) {}
#endif

extern "C" JNIEXPORT jlong JNICALL
Java_com_video_sdk_CameraCapture_nativeCreate(JNIEnv* env, jobject thiz) {
    try {
        auto capture = new video_sdk::modules::CameraCapture();
        return reinterpret_cast<jlong>(capture);
    } catch (const std::exception& e) {
        video_sdk::jni::JniExceptionHandler::throwKotlinException(env, "com/video/sdk/VideoSdkError$InitializationError", e.what());
        return 0;
    }
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_video_sdk_CameraCapture_nativeInitialize(JNIEnv* env, jobject thiz, jlong handle, jint width, jint height, jint fps) {
    auto capture = reinterpret_cast<video_sdk::modules::CameraCapture*>(handle);
    if (!capture) return JNI_FALSE;
    return capture->initialize(width, height, fps) ? JNI_TRUE : JNI_FALSE;
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_CameraCapture_nativeSetPreviewSurface(JNIEnv* env, jobject thiz, jlong handle, jobject surface) {
    auto capture = reinterpret_cast<video_sdk::modules::CameraCapture*>(handle);
    if (!capture) return;

    if (surface == nullptr) {
        capture->setPreviewSurface(nullptr);
    } else {
        ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
        capture->setPreviewSurface(window);
        // 不立即释放 window 引用，底层需要持有该对象在 EGLWindow 中使用。
        // 在完整的生产环境中，EGLWindow 被销毁或者切换预览 Surface 时
        // 应该负责调用 ANativeWindow_release。
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_CameraCapture_nativeStartPreview(JNIEnv* env, jobject thiz, jlong handle) {
    auto capture = reinterpret_cast<video_sdk::modules::CameraCapture*>(handle);
    if (capture) {
        capture->startPreview();
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_CameraCapture_nativeStopPreview(JNIEnv* env, jobject thiz, jlong handle) {
    auto capture = reinterpret_cast<video_sdk::modules::CameraCapture*>(handle);
    if (capture) {
        capture->stopPreview();
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_CameraCapture_nativeStartRecording(JNIEnv* env, jobject thiz, jlong handle, jstring outputPath) {
    auto capture = reinterpret_cast<video_sdk::modules::CameraCapture*>(handle);
    if (!capture) return;

    const char* pathChars = env->GetStringUTFChars(outputPath, nullptr);
    if (!pathChars) {
        video_sdk::jni::JniExceptionHandler::throwKotlinException(env, "com/video/sdk/VideoSdkError$JniEnvError", "Failed to get string UTF chars");
        return;
    }

    try {
        std::string path(pathChars);
        capture->startRecording(path);
    } catch (const std::exception& e) {
        video_sdk::jni::JniExceptionHandler::throwKotlinException(env, "com/video/sdk/VideoSdkError$EngineError", e.what());
    }

    env->ReleaseStringUTFChars(outputPath, pathChars);
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_CameraCapture_nativeStopRecording(JNIEnv* env, jobject thiz, jlong handle) {
    auto capture = reinterpret_cast<video_sdk::modules::CameraCapture*>(handle);
    if (capture) {
        capture->stopRecording();
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_CameraCapture_nativeDestroy(JNIEnv* env, jobject thiz, jlong handle) {
    auto capture = reinterpret_cast<video_sdk::modules::CameraCapture*>(handle);
    if (capture) {
        delete capture;
    }
}
