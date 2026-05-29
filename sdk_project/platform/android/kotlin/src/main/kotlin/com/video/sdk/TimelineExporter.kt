package com.video.sdk

class TimelineExporter {
    private var nativeHandle: Long = 0

    init {
        System.loadLibrary("VideoSDKJni")
        nativeHandle = nativeCreate()
    }

    fun startExport() {
        nativeStartExport(nativeHandle)
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
    private external fun nativeStartExport(handle: Long)
    private external fun nativeCancelExport(handle: Long)
    private external fun nativeGetProgress(handle: Long): Float
    private external fun nativeDestroy(handle: Long)
}
