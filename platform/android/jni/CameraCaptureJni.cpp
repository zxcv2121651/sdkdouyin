#include "jni.h"
#include "video_sdk_c_api.h"
#include "android/native_window_jni.h"

extern "C" JNIEXPORT jlong JNICALL
Java_com_video_sdk_CameraCapture_nativeCreate(JNIEnv* env, jobject thiz) {
    VS_CameraHandle handle = nullptr;
    if (vs_camera_create(&handle) == VS_SUCCESS) {
        return reinterpret_cast<jlong>(handle);
    }
    return 0;
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_CameraCapture_nativePushOESTexture(JNIEnv* env, jobject thiz, jlong handle, jint textureId, jint width, jint height, jlong timestampNs, jfloatArray matrix) {
    if (!handle) return;
    VS_CameraHandle cam = reinterpret_cast<VS_CameraHandle>(handle);

    float transformMatrix[16];
    if (matrix != nullptr) {
        reinterpret_cast<_JNIEnv*>(env)->GetFloatArrayRegion(matrix, 0, 16, transformMatrix);
    } else {
        for(int i=0; i<16; i++) transformMatrix[i] = (i%5 == 0) ? 1.0f : 0.0f;
    }

    vs_camera_push_oes_texture(cam, static_cast<uint32_t>(textureId), width, height, timestampNs, transformMatrix);
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_CameraCapture_nativeDestroy(JNIEnv* env, jobject thiz, jlong handle) {
    if (handle) {
        VS_CameraHandle cam = reinterpret_cast<VS_CameraHandle>(handle);
        vs_camera_destroy(cam);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_CameraCapture_nativeSetPreviewSurface(JNIEnv* env, jobject thiz, jlong handle, jobject surface) {
    if (handle) {
        VS_CameraHandle cam = reinterpret_cast<VS_CameraHandle>(handle);
        void* window = nullptr;
        if (surface != nullptr) {
            // ANativeWindow_fromSurface mock might need env
            // Mock definition handles it
        }
        vs_camera_set_preview_window(cam, window);
    }
}
