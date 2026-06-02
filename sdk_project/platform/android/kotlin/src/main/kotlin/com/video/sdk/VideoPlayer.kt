package com.video.sdk

import android.view.Surface

/**
 * 视频播放器核心 SDK 门面类。
 * 负责与底层 C++ 引擎（VideoPlayer）交互，提供基于硬件加速的零拷贝视频播放能力。
 * 使用 JNI (Java Native Interface) 管理 C++ 对象生命周期。
 */
class VideoPlayer {
    // 保存底层 C++ std::shared_ptr<VideoPlayer> 的内存地址指针
    private var nativeHandle: Long = 0

    init {
        // 加载 C++ 编译生成的动态链接库
        System.loadLibrary("VideoSDKJni")
        nativeHandle = nativeCreate()
    }

    /**
     * 将 Android 原生屏幕 (Surface) 绑定到 C++ 底层。
     * 底层通过 ANativeWindow 接口直接把 GPU 渲染好的画面推送到此 Surface 上，
     * 避免了 CPU 参与像素拷贝，实现极致性能。
     *
     * @param surface Android 的 Surface 对象（通常来自 SurfaceView 或 TextureView）
     */
    fun setSurface(surface: Surface?) {
        nativeSetSurface(nativeHandle, surface)
    }

    /**
     * 准备视频资源进行解码和播放。
     *
     * @param sourceUrl 视频本地文件路径或网络流地址
     * @return 准备成功返回 true，失败返回 false
     * @throws VideoSdkError 底层 C++ 如果抛出异常，会被 JNI 拦截并转换为该 Kotlin 异常
     */
    @Throws(VideoSdkError::class)
    fun prepare(sourceUrl: String): Boolean {
        return nativePrepare(nativeHandle, sourceUrl)
    }

    /**
     * 开始或恢复播放。
     * 调用后底层 AVSyncClock (主时钟) 开始运转，视频帧开始调度。
     */
    fun play() {
        nativePlay(nativeHandle)
    }

    /**
     * 暂停播放。
     */
    fun pause() {
        nativePause(nativeHandle)
    }

    /**
     * 进度跳转 (Seek)。
     *
     * @param timestampMs 目标时间戳（毫秒）
     */
    fun seekTo(timestampMs: Long) {
        nativeSeekTo(nativeHandle, timestampMs)
    }

    /**
     * 获取当前播放器的进度。
     *
     * @return 当前播放进度时间戳（毫秒）
     */
    fun getCurrentPosition(): Long {
        return nativeGetCurrentPosition(nativeHandle)
    }

    /**
     * 查询底层播放器是否处于播放状态。
     *
     * @return 正在播放返回 true，否则返回 false
     */
    fun isPlaying(): Boolean {
        return nativeIsPlaying(nativeHandle)
    }

    /**
     * 销毁底层 C++ 实例对象并释放相关内存。
     * 必须在 UI 销毁（如 Activity/Fragment onDestroy）时调用，防止内存泄漏。
     */
    fun destroy() {
        if (nativeHandle != 0L) {
            nativeDestroy(nativeHandle)
            nativeHandle = 0L
        }
    }

    // --- Native JNI 接口声明 ---
    private external fun nativeCreate(): Long
    private external fun nativeSetSurface(handle: Long, surface: Surface?)
    private external fun nativePrepare(handle: Long, sourceUrl: String): Boolean
    private external fun nativePlay(handle: Long)
    private external fun nativePause(handle: Long)
    private external fun nativeSeekTo(handle: Long, timestampMs: Long)
    private external fun nativeGetCurrentPosition(handle: Long): Long
    private external fun nativeIsPlaying(handle: Long): Boolean
    private external fun nativeDestroy(handle: Long)
}
