package com.video.sdk

import android.view.Surface

/**
 * 极速录制采集模块的上层 Kotlin 门面（Facade）。
 * 封装底层的 JNI 句柄管理。
 */
class CameraCapture {

    // 保存底层 C++ 实例的内存地址
    private var nativeHandle: Long = 0

    init {
        // 在真实 Android 工程中，通常通过 JNI_OnLoad 统一加载并初始化
        // System.loadLibrary("VideoSDKCore")
        nativeHandle = nativeCreate()
    }

    /**
     * 初始化相机参数
     */
    fun initialize(width: Int, height: Int, fps: Int): Boolean {
        return nativeInitialize(nativeHandle, width, height, fps)
    }

    /**
     * 设置预览 Surface（通常从 SurfaceView 拿到）
     */
    fun setPreviewSurface(surface: Surface?) {
        nativeSetPreviewSurface(nativeHandle, surface)
    }

    /**
     * 开始预览
     */
    fun startPreview() {
        nativeStartPreview(nativeHandle)
    }

    /**
     * 停止预览
     */
    fun stopPreview() {
        nativeStopPreview(nativeHandle)
    }

    /**
     * 开始录制到文件
     */
    fun startRecording(outputPath: String) {
        nativeStartRecording(nativeHandle, outputPath)
    }

    /**
     * 停止录制
     */
    fun stopRecording() {
        nativeStopRecording(nativeHandle)
    }

    /**
     * 销毁底层 C++ 实例
     */
    fun destroy() {
        if (nativeHandle != 0L) {
            nativeDestroy(nativeHandle)
            nativeHandle = 0L
        }
    }

    // --- Native 接口声明 ---
    private external fun nativeCreate(): Long
    private external fun nativeInitialize(handle: Long, width: Int, height: Int, fps: Int): Boolean
    private external fun nativeSetPreviewSurface(handle: Long, surface: Surface?)
    private external fun nativeStartPreview(handle: Long)
    private external fun nativeStopPreview(handle: Long)
    private external fun nativeStartRecording(handle: Long, outputPath: String)
    private external fun nativeStopRecording(handle: Long)
    private external fun nativeDestroy(handle: Long)
}
