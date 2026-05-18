package com.video.sdk

/**
 * Sealed class representing domain-specific SDK errors mapped from C++ JNI layer.
 */
sealed class VideoSdkError(val code: Int, message: String) : Exception(message) {
    class InitializationError(message: String) : VideoSdkError(1001, message)
    class JniEnvError(message: String) : VideoSdkError(1002, message)
    class EngineError(code: Int, message: String) : VideoSdkError(code, message)
    class OomError(message: String) : VideoSdkError(2001, message)
}
