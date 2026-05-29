package com.video.sdk

class TimelineExporter {
    private var nativeHandle: Long = 0

    init {
        System.loadLibrary("VideoSDKJni")
        nativeHandle = nativeCreate()
    }

    fun startExport() {
        if (nativeHandle != 0L) {
            nativeStartExport(nativeHandle)
        }
    }

    fun cancelExport() {
        if (nativeHandle != 0L) {
            nativeCancelExport(nativeHandle)
        }
    }

    fun getProgress(): Float {
        return if (nativeHandle != 0L) {
            nativeGetProgress(nativeHandle)
        } else 0.0f
    }

    fun destroy() {
        if (nativeHandle != 0L) {
            nativeDestroy(nativeHandle)
            nativeHandle = 0L
        }
    }

    private external fun nativeCreate(): Long
    private external fun nativeDestroy(handle: Long)
    private external fun nativeStartExport(handle: Long)
    private external fun nativeCancelExport(handle: Long)
    private external fun nativeGetProgress(handle: Long): Float
}
