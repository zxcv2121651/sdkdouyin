#pragma once
#include <jni.h>
#include <string>

namespace video_sdk {
namespace jni {

/**
 * @brief 异常处理助手类，用于将 C++ 的异常或错误映射为 Kotlin/Java 的异常。
 * JNI 方法应该返回标准的 jint 错误码，或者通过这个助手类抛出 JVM 异常，严禁底层直接触发 native 崩溃。
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
