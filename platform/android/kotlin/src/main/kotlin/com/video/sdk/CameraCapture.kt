package com.video.sdk

import android.view.Surface

/**
 * 摄像头采集核心 SDK 门面类。
 * 现已重构为“Kotlin 层采集 + C++ 层渲染”的混合架构。
 * 负责在 Java/Kotlin 层接收系统相机的 OES 纹理，并将其透传给 C++ 引擎。
 */
class CameraCapture {
    private var nativeHandle: Long = 0

    init {
        System.loadLibrary("VideoSDKJni")
        nativeHandle = nativeCreate()
    }

    /**
     * 将 Android 层产生的 OES 纹理 ID 推送给 C++ 渲染管线
     * @param textureId OpenGL OES 纹理 ID
     * @param width 画面宽度
     * @param height 画面高度
     * @param timestampNs 纹理产生的时间戳（纳秒）
     * @param transformMatrix SurfaceTexture 的 4x4 变换矩阵 (长度16的浮点数组)
     */
    fun pushOESTexture(textureId: Int, width: Int, height: Int, timestampNs: Long, transformMatrix: FloatArray?) {
        if (nativeHandle != 0L) {
            nativePushOESTexture(nativeHandle, textureId, width, height, timestampNs, transformMatrix)
        }
    }

    /**
     * [保留给历史/预览接口] 设置直接在 C++ 层进行预览的 Surface。
     */
    fun setPreviewSurface(surface: Surface?) {
        if (nativeHandle != 0L) {
            nativeSetPreviewSurface(nativeHandle, surface)
        }
    }

    /**
     * 销毁底层实例
     */
    fun destroy() {
        if (nativeHandle != 0L) {
            nativeDestroy(nativeHandle)
            nativeHandle = 0L
        }
    }

    // --- Native JNI 接口声明 ---
    private external fun nativeCreate(): Long
    private external fun nativePushOESTexture(handle: Long, textureId: Int, width: Int, height: Int, timestampNs: Long, transformMatrix: FloatArray?)
    private external fun nativeSetPreviewSurface(handle: Long, surface: Surface?)
    private external fun nativeDestroy(handle: Long)
}
