package com.video.sdk

class VideoPlayer {
    private var nativeHandle: Long = 0

    init {
        System.loadLibrary("VideoSDKJni")
        nativeHandle = nativeCreate()
    }

    @Throws(VideoSdkError::class)
    fun prepare(sourceUrl: String): Boolean {
        return nativePrepare(nativeHandle, sourceUrl)
    }

    fun play() {
        nativePlay(nativeHandle)
    }

    fun pause() {
        nativePause(nativeHandle)
    }

    fun seekTo(timestampMs: Long) {
        nativeSeekTo(nativeHandle, timestampMs)
    }

    fun getCurrentPosition(): Long {
        return nativeGetCurrentPosition(nativeHandle)
    }

    fun isPlaying(): Boolean {
        return nativeIsPlaying(nativeHandle)
    }

    fun destroy() {
        if (nativeHandle != 0L) {
            nativeDestroy(nativeHandle)
            nativeHandle = 0L
        }
    }

    private external fun nativeCreate(): Long
    private external fun nativePrepare(handle: Long, sourceUrl: String): Boolean
    private external fun nativePlay(handle: Long)
    private external fun nativePause(handle: Long)
    private external fun nativeSeekTo(handle: Long, timestampMs: Long)
    private external fun nativeGetCurrentPosition(handle: Long): Long
    private external fun nativeIsPlaying(handle: Long): Boolean
    private external fun nativeDestroy(handle: Long)
}
