# API 接口参考文档 (API Reference)

本文档列出了供前端 UI 开发者（Android / Kotlin）调用的核心门面类 API。所有的底层 C++ 复杂性均已被这些 Kotlin 类封装屏蔽。

所有 Kotlin SDK 类位于包名 `com.video.sdk` 下。

---

## 1. CameraCapture (摄像采集类)
用于控制设备硬件摄像头并将画面直接喂给 SDK 渲染管线。

### 核心方法
*   **`fun setPreviewSurface(surface: Surface?)`**
    *   **描述**：绑定用于预览相机画面的屏幕画板。通常传入 `SurfaceView` 或 `TextureView` 的 Surface。
*   **`fun initialize(width: Int, height: Int, fps: Int): Boolean`**
    *   **描述**：初始化相机参数并尝试打开相机。
    *   **参数**：
        *   `width`, `height`: 期望的分辨率（如 1080, 1920）
        *   `fps`: 期望的帧率（如 30）
    *   **返回**：是否初始化成功
*   **`fun startPreview()`**
    *   **描述**：开始采集画面并推流。
*   **`fun stopPreview()`**
    *   **描述**：停止采集画面。
*   **`fun switchCamera()`**
    *   **描述**：在前后摄像头之间翻转切换。
*   **`fun destroy()`**
    *   **描述**：销毁实例，彻底释放硬件相机占用。必须在 UI `onDispose` 或 `onDestroy` 中调用。

---

## 2. VideoPlayer (视频播放与预览类)
用于在剪辑工作台中实时预览由多轨道、多滤镜组装而成的时间线效果。

### 核心方法
*   **`fun setSurface(surface: Surface?)`**
    *   **描述**：绑定 Android 屏幕 Surface，底层 GPU 将特效处理完的画面直接上屏。
*   **`fun prepare(sourceUrl: String): Boolean`**
    *   **描述**：解析资源并拉起底层解码器（支持本地文件路径）。
*   **`fun play()`**
    *   **描述**：开始驱动主时钟，视频画面开始刷新。
*   **`fun pause()`**
    *   **描述**：暂停画面刷新。
*   **`fun seekTo(timestampMs: Long)`**
    *   **描述**：精确跳转到某一毫秒。
*   **`fun getCurrentPosition(): Long`**
    *   **描述**：获取当前播放进度（毫秒）。
*   **`fun isPlaying(): Boolean`**
    *   **描述**：返回当前是否处于播放状态。
*   **`fun destroy()`**
    *   **描述**：释放播放器及底层的解码器、渲染器内存。

---

## 3. TimelineExporter (视频导出与打包类)
用于将编辑好的视频工程导出为 MP4 成片文件。

### 核心方法
*   **`fun startExport(outputPath: String = "")`**
    *   **描述**：触发后台导出引擎，启动硬件编码器并写入数据。
    *   **参数**：`outputPath` - 要生成的 MP4 文件的绝对路径。如果不传，底层默认保存至 `/sdcard/output.mp4`。
*   **`fun cancelExport()`**
    *   **描述**：中断当前的导出任务并清理临时文件缓冲。
*   **`fun getProgress(): Float`**
    *   **描述**：查询当前导出的总体进度。UI 层应使用定时器或协程轮询此方法。
    *   **返回**：浮点数进度，区间为 `[0.0, 1.0]`，代表 `0% ~ 100%`。
*   **`fun destroy()`**
    *   **描述**：释放导出引擎资源。

---

## 4. 异常处理 (VideoSdkError)
底层的所有崩溃和错误（如解码器失败、内存溢出），绝不会导致 App 直接闪退 (Crash)。JNI 桥接层会拦截异常并统一抛出 Kotlin `Sealed Class`。

*   `InitializationError(message)`：模块启动/内存分配失败。
*   `EngineError(code, message)`：渲染管线或编解码器报错。
*   `OomError(message)`：GPU 显存或系统内存爆满。

*建议在调用 `prepare` 或 `initialize` 时使用 `try-catch(e: VideoSdkError)` 包裹处理。*
