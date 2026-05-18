#pragma once
#include <jni.h>
#include <string>

namespace video_sdk {
namespace jni {

/**
 * @brief Helper to translate C++ exceptions/errors into Kotlin domain exceptions.
 * Ensure JNI methods return standard jint error codes or throw JVM exceptions instead of native crashes.
 */
class JniExceptionHandler {
public:
    static void throwKotlinException(JNIEnv* env, const std::string& className, const std::string& message) {
        jclass exClass = env->FindClass(className.c_str());
        if (exClass != nullptr) {
            env->ThrowNew(exClass, message.c_str());
        }
    }
};

} // namespace jni
} // namespace video_sdk
