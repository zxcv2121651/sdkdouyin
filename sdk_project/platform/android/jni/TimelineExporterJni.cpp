#include <jni.h>
#include <memory>
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
Java_com_video_sdk_TimelineExporter_nativeDestroy(JNIEnv* env, jobject thiz, jlong handle) {
    auto exporterPtr = reinterpret_cast<std::shared_ptr<video_sdk::hal::TimelineExporter>*>(handle);
    if (exporterPtr) {
        delete exporterPtr;
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_TimelineExporter_nativeStartExport(JNIEnv* env, jobject thiz, jlong handle) {
    auto exporterPtr = reinterpret_cast<std::shared_ptr<video_sdk::hal::TimelineExporter>*>(handle);
    if (exporterPtr && *exporterPtr) {
        (*exporterPtr)->startExport();
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_video_sdk_TimelineExporter_nativeCancelExport(JNIEnv* env, jobject thiz, jlong handle) {
    auto exporterPtr = reinterpret_cast<std::shared_ptr<video_sdk::hal::TimelineExporter>*>(handle);
    if (exporterPtr && *exporterPtr) {
        (*exporterPtr)->cancelExport();
    }
}

extern "C" JNIEXPORT jfloat JNICALL
Java_com_video_sdk_TimelineExporter_nativeGetProgress(JNIEnv* env, jobject thiz, jlong handle) {
    auto exporterPtr = reinterpret_cast<std::shared_ptr<video_sdk::hal::TimelineExporter>*>(handle);
    if (exporterPtr && *exporterPtr) {
        return static_cast<jfloat>((*exporterPtr)->getProgress());
    }
    return 0.0f;
}
