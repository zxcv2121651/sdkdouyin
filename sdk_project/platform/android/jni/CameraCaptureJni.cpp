#include <jni.h>
#include <memory>
#include "JniExceptionHandler.h"
#include "modules/camera/CameraCapture.h"

extern "C" JNIEXPORT jlong JNICALL
Java_com_video_sdk_CameraCapture_nativeCreate(JNIEnv* env, jobject thiz) {
    try {
        auto camera = std::make_shared<video_sdk::modules::CameraCapture>();
        return reinterpret_cast<jlong>(new std::shared_ptr<video_sdk::modules::CameraCapture>(camera));
    } catch (const std::exception& e) {
        video_sdk::jni::JniExceptionHandler::throwKotlinException(env, "com/video/sdk/VideoSdkError$InitializationError", e.what());
        return 0;
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_CameraCapture_nativeDestroy(JNIEnv* env, jobject thiz, jlong handle) {
    auto cameraPtr = reinterpret_cast<std::shared_ptr<video_sdk::modules::CameraCapture>*>(handle);
    if (cameraPtr) {
        delete cameraPtr;
    }
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_video_sdk_CameraCapture_nativeInitialize(JNIEnv* env, jobject thiz, jlong handle, jint width, jint height, jint fps) {
    auto cameraPtr = reinterpret_cast<std::shared_ptr<video_sdk::modules::CameraCapture>*>(handle);
    if (cameraPtr && *cameraPtr) {
        return (*cameraPtr)->initialize(static_cast<int>(width), static_cast<int>(height), static_cast<int>(fps)) ? JNI_TRUE : JNI_FALSE;
    }
    return JNI_FALSE;
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_CameraCapture_nativeStartPreview(JNIEnv* env, jobject thiz, jlong handle) {
    auto cameraPtr = reinterpret_cast<std::shared_ptr<video_sdk::modules::CameraCapture>*>(handle);
    if (cameraPtr && *cameraPtr) {
        (*cameraPtr)->startPreview();
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_CameraCapture_nativeStopPreview(JNIEnv* env, jobject thiz, jlong handle) {
    auto cameraPtr = reinterpret_cast<std::shared_ptr<video_sdk::modules::CameraCapture>*>(handle);
    if (cameraPtr && *cameraPtr) {
        (*cameraPtr)->stopPreview();
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_CameraCapture_nativeSwitchCamera(JNIEnv* env, jobject thiz, jlong handle) {
    auto cameraPtr = reinterpret_cast<std::shared_ptr<video_sdk::modules::CameraCapture>*>(handle);
    if (cameraPtr && *cameraPtr) {
        (*cameraPtr)->switchCamera();
    }
}
