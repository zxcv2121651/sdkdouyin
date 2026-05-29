package com.video.sdk

class CameraCapture {
    private var nativeHandle: Long = 0

    init {
        System.loadLibrary("VideoSDKJni")
        nativeHandle = nativeCreate()
    }

    fun initialize(width: Int, height: Int, fps: Int): Boolean {
        return if (nativeHandle != 0L) {
            nativeInitialize(nativeHandle, width, height, fps)
        } else false
    }

    fun startPreview() {
        if (nativeHandle != 0L) {
            nativeStartPreview(nativeHandle)
        }
    }

    fun stopPreview() {
        if (nativeHandle != 0L) {
            nativeStopPreview(nativeHandle)
        }
    }

    fun switchCamera() {
        if (nativeHandle != 0L) {
            nativeSwitchCamera(nativeHandle)
        }
    }

    fun destroy() {
        if (nativeHandle != 0L) {
            nativeDestroy(nativeHandle)
            nativeHandle = 0L
        }
    }

    private external fun nativeCreate(): Long
    private external fun nativeDestroy(handle: Long)
    private external fun nativeInitialize(handle: Long, width: Int, height: Int, fps: Int): Boolean
    private external fun nativeStartPreview(handle: Long)
    private external fun nativeStopPreview(handle: Long)
    private external fun nativeSwitchCamera(handle: Long)
}
