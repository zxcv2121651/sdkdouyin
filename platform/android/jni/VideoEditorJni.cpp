#include <jni.h>
#include <string>
#include <memory>
#include "JniExceptionHandler.h"
#include "modules/editor/VideoEditor.h"

// 保存全局的 C++ 实例指针 (实际开发中应该封装在句柄中或绑定在 Kotlin 对象的 long 类型成员中)
static std::shared_ptr<video_sdk::modules::VideoEditor> g_editor;

extern "C" JNIEXPORT jlong JNICALL
Java_com_video_sdk_VideoEditor_nativeCreate(JNIEnv* env, jobject thiz) {
    try {
        auto editor = std::make_shared<video_sdk::modules::VideoEditor>();
        // 将 std::shared_ptr 转换为裸指针返回给 Java 层保存
        return reinterpret_cast<jlong>(new std::shared_ptr<video_sdk::modules::VideoEditor>(editor));
    } catch (const std::exception& e) {
        video_sdk::jni::JniExceptionHandler::throwKotlinException(env, "com/video/sdk/VideoSdkError$InitializationError", e.what());
        return 0;
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_VideoEditor_nativeImportVideo(JNIEnv* env, jobject thiz, jlong handle, jstring filePath, jlong startTimeMs) {
    auto editorPtr = reinterpret_cast<std::shared_ptr<video_sdk::modules::VideoEditor>*>(handle);
    if (!editorPtr || !(*editorPtr)) {
        video_sdk::jni::JniExceptionHandler::throwKotlinException(env, "com/video/sdk/VideoSdkError$EngineError", "Editor instance is null");
        return;
    }

    const char* pathChars = env->GetStringUTFChars(filePath, nullptr);
    if (!pathChars) {
        // JNI 安全红线: env->GetStringUTFChars 必须检查空指针
        video_sdk::jni::JniExceptionHandler::throwKotlinException(env, "com/video/sdk/VideoSdkError$JniEnvError", "Failed to get string UTF chars");
        return;
    }

    try {
        std::string path(pathChars);
        (*editorPtr)->importVideo(path, static_cast<int64_t>(startTimeMs));
    } catch (const std::exception& e) {
        video_sdk::jni::JniExceptionHandler::throwKotlinException(env, "com/video/sdk/VideoSdkError$EngineError", e.what());
    }

    env->ReleaseStringUTFChars(filePath, pathChars);
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_VideoEditor_nativeDestroy(JNIEnv* env, jobject thiz, jlong handle) {
    auto editorPtr = reinterpret_cast<std::shared_ptr<video_sdk::modules::VideoEditor>*>(handle);
    if (editorPtr) {
        delete editorPtr;
    }
}
