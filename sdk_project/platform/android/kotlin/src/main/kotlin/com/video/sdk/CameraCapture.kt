package com.video.sdk

import android.view.Surface

class CameraCapture {
    private var nativeHandle: Long = 0

    init {
        System.loadLibrary("VideoSDKJni")
        nativeHandle = nativeCreate()
    }

    fun setPreviewSurface(surface: Surface?) {
        nativeSetPreviewSurface(nativeHandle, surface)
    }

    @Throws(VideoSdkError::class)
    fun initialize(width: Int, height: Int, fps: Int): Boolean {
        return nativeInitialize(nativeHandle, width, height, fps)
    }

    fun startPreview() {
        nativeStartPreview(nativeHandle)
    }

    fun stopPreview() {
        nativeStopPreview(nativeHandle)
    }

    fun switchCamera() {
        nativeSwitchCamera(nativeHandle)
    }

    fun destroy() {
        if (nativeHandle != 0L) {
            nativeDestroy(nativeHandle)
            nativeHandle = 0L
        }
    }

    private external fun nativeCreate(): Long
    private external fun nativeSetPreviewSurface(handle: Long, surface: Surface?)
    private external fun nativeInitialize(handle: Long, width: Int, height: Int, fps: Int): Boolean
    private external fun nativeStartPreview(handle: Long)
    private external fun nativeStopPreview(handle: Long)
    private external fun nativeSwitchCamera(handle: Long)
    private external fun nativeDestroy(handle: Long)
}
