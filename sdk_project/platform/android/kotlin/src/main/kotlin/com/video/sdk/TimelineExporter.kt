package com.video.sdk

/**
 * 视频离线导出核心 SDK 门面类。
 * 驱动底层引擎进行脱离屏幕刷新率限制的极速渲染（Headless Rendering），
 * 并调用硬件编码器 (Android MediaCodec) 和复用器 (MediaMuxer) 将视频流压制打包为 MP4 文件。
 */
class TimelineExporter {
    private var nativeHandle: Long = 0

    init {
        System.loadLibrary("VideoSDKJni")
        nativeHandle = nativeCreate()
    }

    /**
     * 启动视频后台导出任务。
     * 底层会启动独立的 MessageLoop 工作线程，通过状态机推进渲染和硬编，不阻塞 UI 线程。
     *
     * @param outputPath MP4 视频保存的绝对路径。如果传空字符串，底层将使用默认路径（如 /sdcard/output.mp4）。
     */
    fun startExport(outputPath: String = "") {
        nativeStartExport(nativeHandle, outputPath)
    }

    /**
     * 中断当前的导出任务并安全清理编码器资源。
     */
    fun cancelExport() {
        nativeCancelExport(nativeHandle)
    }

    /**
     * 获取实时的视频导出进度。
     * UI 层可以使用 Kotlin Coroutines 定时轮询此接口来更新界面的进度条。
     *
     * @return 当前进度，取值范围 [0.0, 1.0]
     */
    fun getProgress(): Float {
        return nativeGetProgress(nativeHandle)
    }

    /**
     * 销毁底层 C++ 实例。
     */
    fun destroy() {
        if (nativeHandle != 0L) {
            nativeDestroy(nativeHandle)
            nativeHandle = 0L
        }
    }

    // --- Native JNI 接口声明 ---
    private external fun nativeCreate(): Long
    private external fun nativeStartExport(handle: Long, outputPath: String)
    private external fun nativeCancelExport(handle: Long)
    private external fun nativeGetProgress(handle: Long): Float
    private external fun nativeDestroy(handle: Long)
}
