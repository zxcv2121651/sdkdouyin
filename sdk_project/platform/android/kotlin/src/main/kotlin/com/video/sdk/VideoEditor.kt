package com.video.sdk

/**
 * 视频剪辑器的上层 Kotlin 门面（Facade）。
 * 封装底层的 JNI 句柄管理。
 */
class VideoEditor {

    // 保存底层 C++ std::shared_ptr 的内存地址
    private var nativeHandle: Long = 0

    init {
        System.loadLibrary("VideoSDKJni")
        nativeHandle = nativeCreate()
    }

    /**
     * 导入视频到时间线
     * @param filePath 视频本地路径
     * @param startTimeMs 时间线起始位置 (毫秒)
     */
    @Throws(VideoSdkError::class)
    fun importVideo(filePath: String, startTimeMs: Long) {
        nativeImportVideo(nativeHandle, filePath, startTimeMs)
    }

    /**
     * 销毁底层的 C++ 实例释放内存
     */
    fun destroy() {
        if (nativeHandle != 0L) {
            nativeDestroy(nativeHandle)
            nativeHandle = 0L
        }
    }

    // --- Native 接口声明 ---
    private external fun nativeCreate(): Long
    private external fun nativeImportVideo(handle: Long, filePath: String, startTimeMs: Long)
    private external fun nativeDestroy(handle: Long)
}
