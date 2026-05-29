#include <jni.h>
#include <memory>
#include <string>
#include "JniExceptionHandler.h"
#include "modules/player/VideoPlayer.h"

extern "C" JNIEXPORT jlong JNICALL
Java_com_video_sdk_VideoPlayer_nativeCreate(JNIEnv* env, jobject thiz) {
    try {
        auto player = std::make_shared<video_sdk::modules::VideoPlayer>();
        return reinterpret_cast<jlong>(new std::shared_ptr<video_sdk::modules::VideoPlayer>(player));
    } catch (const std::exception& e) {
        video_sdk::jni::JniExceptionHandler::throwKotlinException(env, "com/video/sdk/VideoSdkError$InitializationError", e.what());
        return 0;
    }
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_video_sdk_VideoPlayer_nativePrepare(JNIEnv* env, jobject thiz, jlong handle, jstring sourceUrl) {
    auto playerPtr = reinterpret_cast<std::shared_ptr<video_sdk::modules::VideoPlayer>*>(handle);
    if (!playerPtr || !(*playerPtr)) {
        video_sdk::jni::JniExceptionHandler::throwKotlinException(env, "com/video/sdk/VideoSdkError$EngineError", "VideoPlayer instance is null");
        return JNI_FALSE;
    }

    const char* urlChars = env->GetStringUTFChars(sourceUrl, nullptr);
    if (!urlChars) {
        video_sdk::jni::JniExceptionHandler::throwKotlinException(env, "com/video/sdk/VideoSdkError$JniEnvError", "Failed to get string UTF chars");
        return JNI_FALSE;
    }

    jboolean result = JNI_FALSE;
    try {
        std::string url(urlChars);
        result = (*playerPtr)->prepare(url) ? JNI_TRUE : JNI_FALSE;
    } catch (const std::exception& e) {
        video_sdk::jni::JniExceptionHandler::throwKotlinException(env, "com/video/sdk/VideoSdkError$EngineError", e.what());
    }

    env->ReleaseStringUTFChars(sourceUrl, urlChars);
    return result;
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_VideoPlayer_nativePlay(JNIEnv* env, jobject thiz, jlong handle) {
    auto playerPtr = reinterpret_cast<std::shared_ptr<video_sdk::modules::VideoPlayer>*>(handle);
    if (playerPtr && (*playerPtr)) {
        (*playerPtr)->play();
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_VideoPlayer_nativePause(JNIEnv* env, jobject thiz, jlong handle) {
    auto playerPtr = reinterpret_cast<std::shared_ptr<video_sdk::modules::VideoPlayer>*>(handle);
    if (playerPtr && (*playerPtr)) {
        (*playerPtr)->pause();
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_VideoPlayer_nativeSeekTo(JNIEnv* env, jobject thiz, jlong handle, jlong timestampMs) {
    auto playerPtr = reinterpret_cast<std::shared_ptr<video_sdk::modules::VideoPlayer>*>(handle);
    if (playerPtr && (*playerPtr)) {
        (*playerPtr)->seekTo(static_cast<int64_t>(timestampMs));
    }
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_video_sdk_VideoPlayer_nativeGetCurrentPosition(JNIEnv* env, jobject thiz, jlong handle) {
    auto playerPtr = reinterpret_cast<std::shared_ptr<video_sdk::modules::VideoPlayer>*>(handle);
    if (playerPtr && (*playerPtr)) {
        return static_cast<jlong>((*playerPtr)->getCurrentPosition());
    }
    return 0;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_video_sdk_VideoPlayer_nativeIsPlaying(JNIEnv* env, jobject thiz, jlong handle) {
    auto playerPtr = reinterpret_cast<std::shared_ptr<video_sdk::modules::VideoPlayer>*>(handle);
    if (playerPtr && (*playerPtr)) {
        return (*playerPtr)->isPlaying() ? JNI_TRUE : JNI_FALSE;
    }
    return JNI_FALSE;
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_VideoPlayer_nativeDestroy(JNIEnv* env, jobject thiz, jlong handle) {
    auto playerPtr = reinterpret_cast<std::shared_ptr<video_sdk::modules::VideoPlayer>*>(handle);
    if (playerPtr) {
        delete playerPtr;
    }
}
