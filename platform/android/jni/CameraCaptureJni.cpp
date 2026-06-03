#include <jni.h>
#include <memory>
#include "JniExceptionHandler.h"
#include "modules/camera/CameraCapture.h"

extern "C" JNIEXPORT jlong JNICALL
Java_com_video_sdk_CameraCapture_nativeCreate(JNIEnv* env, jobject thiz) {
    try {
        auto capture = std::make_shared<video_sdk::modules::CameraCapture>();
        return reinterpret_cast<jlong>(new std::shared_ptr<video_sdk::modules::CameraCapture>(capture));
    } catch (const std::exception& e) {
        video_sdk::jni::JniExceptionHandler::throwKotlinException(env, "com/video/sdk/VideoSdkError$InitializationError", e.what());
        return 0;
    }
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_video_sdk_CameraCapture_nativeInitialize(JNIEnv* env, jobject thiz, jlong handle, jint width, jint height, jint fps) {
    auto capturePtr = reinterpret_cast<std::shared_ptr<video_sdk::modules::CameraCapture>*>(handle);
    if (!capturePtr || !(*capturePtr)) {
        video_sdk::jni::JniExceptionHandler::throwKotlinException(env, "com/video/sdk/VideoSdkError$EngineError", "CameraCapture instance is null");
        return JNI_FALSE;
    }

    try {
        return (*capturePtr)->initialize(width, height, fps) ? JNI_TRUE : JNI_FALSE;
    } catch (const std::exception& e) {
        video_sdk::jni::JniExceptionHandler::throwKotlinException(env, "com/video/sdk/VideoSdkError$EngineError", e.what());
        return JNI_FALSE;
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_CameraCapture_nativeStartPreview(JNIEnv* env, jobject thiz, jlong handle) {
    auto capturePtr = reinterpret_cast<std::shared_ptr<video_sdk::modules::CameraCapture>*>(handle);
    if (capturePtr && (*capturePtr)) {
        (*capturePtr)->startPreview();
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_CameraCapture_nativeStopPreview(JNIEnv* env, jobject thiz, jlong handle) {
    auto capturePtr = reinterpret_cast<std::shared_ptr<video_sdk::modules::CameraCapture>*>(handle);
    if (capturePtr && (*capturePtr)) {
        (*capturePtr)->stopPreview();
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_CameraCapture_nativeSwitchCamera(JNIEnv* env, jobject thiz, jlong handle) {
    auto capturePtr = reinterpret_cast<std::shared_ptr<video_sdk::modules::CameraCapture>*>(handle);
    if (capturePtr && (*capturePtr)) {
        (*capturePtr)->switchCamera();
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_CameraCapture_nativeDestroy(JNIEnv* env, jobject thiz, jlong handle) {
    auto capturePtr = reinterpret_cast<std::shared_ptr<video_sdk::modules::CameraCapture>*>(handle);
    if (capturePtr) {
        delete capturePtr;
    }
}

#include <android/native_window_jni.h>

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_CameraCapture_nativeSetPreviewSurface(JNIEnv* env, jobject thiz, jlong handle, jobject surface) {
    auto capturePtr = reinterpret_cast<std::shared_ptr<video_sdk::modules::CameraCapture>*>(handle);
    if (capturePtr && (*capturePtr)) {
        ANativeWindow* window = nullptr;
        if (surface != nullptr) {
            window = ANativeWindow_fromSurface(env, surface);
        }
        (*capturePtr)->setPreviewWindow(window);
    }
}
