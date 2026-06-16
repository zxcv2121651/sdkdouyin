#include "jni.h"
#include "video_sdk_c_api.h"
#include "android/native_window_jni.h"

extern "C" JNIEXPORT jlong JNICALL
Java_com_video_sdk_VideoPlayer_nativeCreate(JNIEnv* env, jobject thiz) {
    VS_PlayerHandle handle = nullptr;
    if (vs_player_create(&handle) == VS_SUCCESS) {
        return reinterpret_cast<jlong>(handle);
    }
    return 0;
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_VideoPlayer_nativeSetSurface(JNIEnv* env, jobject thiz, jlong handle, jobject surface) {
    if (!handle) return;
    VS_PlayerHandle player = reinterpret_cast<VS_PlayerHandle>(handle);
    void* window = nullptr;
    if (surface != nullptr) {
        // window = ANativeWindow_fromSurface(env, surface);
    }
    vs_player_set_window(player, window);
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_VideoPlayer_nativePlay(JNIEnv* env, jobject thiz, jlong handle) {
    if (!handle) return;
    VS_PlayerHandle player = reinterpret_cast<VS_PlayerHandle>(handle);
    vs_player_play(player);
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_VideoPlayer_nativePause(JNIEnv* env, jobject thiz, jlong handle) {
    if (!handle) return;
    VS_PlayerHandle player = reinterpret_cast<VS_PlayerHandle>(handle);
    vs_player_pause(player);
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_VideoPlayer_nativeSeekTo(JNIEnv* env, jobject thiz, jlong handle, jlong timeMs) {
    if (!handle) return;
    VS_PlayerHandle player = reinterpret_cast<VS_PlayerHandle>(handle);
    vs_player_seek(player, timeMs);
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_VideoPlayer_nativeDestroy(JNIEnv* env, jobject thiz, jlong handle) {
    if (handle) {
        VS_PlayerHandle player = reinterpret_cast<VS_PlayerHandle>(handle);
        vs_player_destroy(player);
    }
}
