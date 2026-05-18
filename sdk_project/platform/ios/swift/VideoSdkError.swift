import Foundation

public enum VideoSdkError: Error {
    case initializationError(String)
    case engineError(Int, String)
    case oomError(String)
}
