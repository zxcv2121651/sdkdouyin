package com.video.sdk

/**
 * 这是一个密封类（Sealed class），用于表示从 C++ JNI 层映射过来的 SDK 领域特定错误。
 * 统一了底层的异常向 Kotlin 层的安全抛出。
 */
sealed class VideoSdkError(val code: Int, message: String) : Exception(message) {
    class InitializationError(message: String) : VideoSdkError(1001, message)
    class JniEnvError(message: String) : VideoSdkError(1002, message)
    class EngineError(code: Int, message: String) : VideoSdkError(code, message)
    class OomError(message: String) : VideoSdkError(2001, message)
}
