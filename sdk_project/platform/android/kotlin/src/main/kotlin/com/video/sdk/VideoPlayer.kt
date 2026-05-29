package com.video.sdk

class VideoPlayer {

    private var nativeHandle: Long = 0

    init {
        System.loadLibrary("VideoSDKJni")
        nativeHandle = nativeCreate()
    }

    fun prepare(sourceUrl: String): Boolean {
        return if (nativeHandle != 0L) {
            nativePrepare(nativeHandle, sourceUrl)
        } else false
    }

    fun play() {
        if (nativeHandle != 0L) {
            nativePlay(nativeHandle)
        }
    }

    fun pause() {
        if (nativeHandle != 0L) {
            nativePause(nativeHandle)
        }
    }

    fun seekTo(timestampMs: Long) {
        if (nativeHandle != 0L) {
            nativeSeekTo(nativeHandle, timestampMs)
        }
    }

    fun getCurrentPosition(): Long {
        return if (nativeHandle != 0L) {
            nativeGetCurrentPosition(nativeHandle)
        } else 0L
    }

    fun isPlaying(): Boolean {
        return if (nativeHandle != 0L) {
            nativeIsPlaying(nativeHandle)
        } else false
    }

    fun destroy() {
        if (nativeHandle != 0L) {
            nativeDestroy(nativeHandle)
            nativeHandle = 0L
        }
    }

    private external fun nativeCreate(): Long
    private external fun nativeDestroy(handle: Long)
    private external fun nativePrepare(handle: Long, sourceUrl: String): Boolean
    private external fun nativePlay(handle: Long)
    private external fun nativePause(handle: Long)
    private external fun nativeSeekTo(handle: Long, timestampMs: Long)
    private external fun nativeGetCurrentPosition(handle: Long): Long
    private external fun nativeIsPlaying(handle: Long): Boolean
}
