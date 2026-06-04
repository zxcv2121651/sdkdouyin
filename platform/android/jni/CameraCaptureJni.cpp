#include <jni.h>
#include <memory>
#include <vector>
#include "JniExceptionHandler.h"
#include "modules/camera/CameraCapture.h"
#include <android/native_window_jni.h>

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

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_CameraCapture_nativePushOESTexture(JNIEnv* env, jobject thiz, jlong handle, jint textureId, jint width, jint height, jlong timestampNs, jfloatArray matrix) {
    auto capturePtr = reinterpret_cast<std::shared_ptr<video_sdk::modules::CameraCapture>*>(handle);
    if (!capturePtr || !(*capturePtr)) {
        return;
    }

    float transformMatrix[16];
    if (matrix != nullptr) {
        env->GetFloatArrayRegion(matrix, 0, 16, transformMatrix);
    } else {
        // Identity matrix fallback
        for(int i=0; i<16; i++) transformMatrix[i] = (i%5 == 0) ? 1.0f : 0.0f;
    }

    // Convert nanoseconds (from SurfaceTexture) to milliseconds for internal SDK usage
    int64_t timestampMs = timestampNs / 1000000;

    (*capturePtr)->pushOESTexture(static_cast<uint32_t>(textureId), width, height, timestampMs, transformMatrix);
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_CameraCapture_nativeDestroy(JNIEnv* env, jobject thiz, jlong handle) {
    auto capturePtr = reinterpret_cast<std::shared_ptr<video_sdk::modules::CameraCapture>*>(handle);
    if (capturePtr) {
        delete capturePtr;
    }
}

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
