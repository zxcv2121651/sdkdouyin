package com.video.sdk

import android.view.Surface

/**
 * 摄像头采集核心 SDK 门面类。
 * 封装了底层 C++ 对 Android NDK Camera2 API 的直接调用。
 * 支持极速拉起摄像头并将画面通过硬件直通 (Zero-Copy) 方式送入 GPU 渲染管线。
 */
class CameraCapture {
    private var nativeHandle: Long = 0

    init {
        System.loadLibrary("VideoSDKJni")
        nativeHandle = nativeCreate()
    }

    /**
     * 设置用于实时预览的屏幕画板。
     * 底层获取到 Android Surface 后，会将其直接挂载为 NDK Camera 的输出目标之一。
     *
     * @param surface Android 原生 Surface 对象
     */
    fun setPreviewSurface(surface: Surface?) {
        nativeSetPreviewSurface(nativeHandle, surface)
    }

    /**
     * 初始化摄像头参数。
     *
     * @param width 期望的采集宽度分辨率
     * @param height 期望的采集高度分辨率
     * @param fps 期望的采集帧率
     * @return 初始化是否成功
     * @throws VideoSdkError NDK 相机会话如果创建失败会抛出该异常
     */
    @Throws(VideoSdkError::class)
    fun initialize(width: Int, height: Int, fps: Int): Boolean {
        return nativeInitialize(nativeHandle, width, height, fps)
    }

    /**
     * 开启摄像头采集并推流。
     */
    fun startPreview() {
        nativeStartPreview(nativeHandle)
    }

    /**
     * 停止摄像头采集。
     */
    fun stopPreview() {
        nativeStopPreview(nativeHandle)
    }

    /**
     * 翻转前后摄像头。
     */
    fun switchCamera() {
        nativeSwitchCamera(nativeHandle)
    }

    /**
     * 销毁底层实例，释放硬件相机设备句柄。
     */
    fun destroy() {
        if (nativeHandle != 0L) {
            nativeDestroy(nativeHandle)
            nativeHandle = 0L
        }
    }

    // --- Native JNI 接口声明 ---
    private external fun nativeCreate(): Long
    private external fun nativeSetPreviewSurface(handle: Long, surface: Surface?)
    private external fun nativeInitialize(handle: Long, width: Int, height: Int, fps: Int): Boolean
    private external fun nativeStartPreview(handle: Long)
    private external fun nativeStopPreview(handle: Long)
    private external fun nativeSwitchCamera(handle: Long)
    private external fun nativeDestroy(handle: Long)
}
