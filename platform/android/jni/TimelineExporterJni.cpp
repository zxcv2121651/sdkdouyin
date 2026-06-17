#include <jni.h>
#include <memory>
#include <string>
#include "JniExceptionHandler.h"
#include "hal/exporter/TimelineExporter.h"

extern "C" JNIEXPORT jlong JNICALL
Java_com_video_sdk_TimelineExporter_nativeCreate(JNIEnv* env, jobject thiz) {
    try {
        auto exporter = std::make_shared<video_sdk::hal::TimelineExporter>();
        return reinterpret_cast<jlong>(new std::shared_ptr<video_sdk::hal::TimelineExporter>(exporter));
    } catch (const std::exception& e) {
        video_sdk::jni::JniExceptionHandler::throwKotlinException(env, "com/video/sdk/VideoSdkError$InitializationError", e.what());
        return 0;
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_TimelineExporter_nativeStartExport(JNIEnv* env, jobject thiz, jlong handle, jstring outputPath) {
    auto exporterPtr = reinterpret_cast<std::shared_ptr<video_sdk::hal::TimelineExporter>*>(handle);
    if (!exporterPtr || !(*exporterPtr)) {
        return;
    }

    std::string pathStr = "";
    if (outputPath != nullptr) {
        const char* pathChars = env->GetStringUTFChars(outputPath, nullptr);
        if (pathChars) {
            pathStr = std::string(pathChars);
            env->ReleaseStringUTFChars(outputPath, pathChars);
        }
    }

    (*exporterPtr)->startExport(pathStr);
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_TimelineExporter_nativeCancelExport(JNIEnv* env, jobject thiz, jlong handle) {
    auto exporterPtr = reinterpret_cast<std::shared_ptr<video_sdk::hal::TimelineExporter>*>(handle);
    if (exporterPtr && (*exporterPtr)) {
        (*exporterPtr)->cancelExport();
    }
}

extern "C" JNIEXPORT jfloat JNICALL
Java_com_video_sdk_TimelineExporter_nativeGetProgress(JNIEnv* env, jobject thiz, jlong handle) {
    auto exporterPtr = reinterpret_cast<std::shared_ptr<video_sdk::hal::TimelineExporter>*>(handle);
    if (exporterPtr && (*exporterPtr)) {
        return (*exporterPtr)->getProgress();
    }
    return 0.0f;
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_TimelineExporter_nativeDestroy(JNIEnv* env, jobject thiz, jlong handle) {
    auto exporterPtr = reinterpret_cast<std::shared_ptr<video_sdk::hal::TimelineExporter>*>(handle);
    if (exporterPtr) {
        delete exporterPtr;
    }
}
