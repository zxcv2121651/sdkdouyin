package com.video.sdk

class TimelineExporter {
    private var nativeHandle: Long = 0

    init {
        System.loadLibrary("VideoSDKJni")
        nativeHandle = nativeCreate()
    }

    /**
     * @param outputPath MP4保存路径。如果为空，则使用底层默认路径 /sdcard/output.mp4
     */
    fun startExport(outputPath: String = "") {
        nativeStartExport(nativeHandle, outputPath)
    }

    fun cancelExport() {
        nativeCancelExport(nativeHandle)
    }

    fun getProgress(): Float {
        return nativeGetProgress(nativeHandle)
    }

    fun destroy() {
        if (nativeHandle != 0L) {
            nativeDestroy(nativeHandle)
            nativeHandle = 0L
        }
    }

    private external fun nativeCreate(): Long
    private external fun nativeStartExport(handle: Long, outputPath: String)
    private external fun nativeCancelExport(handle: Long)
    private external fun nativeGetProgress(handle: Long): Float
    private external fun nativeDestroy(handle: Long)
}
