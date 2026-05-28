import Foundation

/**
 * 视频 SDK 的全局错误枚举，与底层 C++ 核心层的状态码进行映射
 */
public enum VideoSdkError: Error {
    case initializationError(String)
    case engineError(Int, String)
    case oomError(String)
}
