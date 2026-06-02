# Video SDK Project (高仿剪映/抖音底层架构)

## 1. 项目概述 (Project Overview)
本项目是一个对标字节跳动 (ByteDance) 和快手 (Kuaishou) 的工业级、跨平台音视频剪辑底层引擎及 Android 演示工程。
它采用了**高内聚、低耦合**的架构设计，C++ 底层负责极致的性能压榨，上层（Android/iOS）负责业务包装与 UI 渲染。

## 2. 核心架构设计 (Core Architecture)

### 2.1 C++ 核心引擎 (Core Engine)
引擎代码全部位于 `core/`, `media/`, `hal/`, `rhi/`, `modules/` 目录下，100% 跨平台 (Android, iOS, Windows, macOS)。

*   **RenderGraph (DAG 渲染管线)**：位于 `core/engine/`。它是一个有向无环图，所有特效（滤镜、转场）都是一个 `RenderNode`。画面在显存（FBO/Texture）中链式流转。
*   **AVSyncClock (音画同步引擎)**：位于 `media/sync/`。摒弃了低效的 `sleep_for` 轮询，采用 `LockFreeRingBuffer` (无锁环形队列，解决 False Sharing) 配合 `MessageLoop` (事件驱动机制) 实现极致零功耗休眠和毫秒级同步调度。
*   **Zero-Copy 零拷贝硬编硬解**：位于 `hal/codec/`。在 Android 端使用 NDK `AMediaCodec` 绑定 `Surface` / `ANativeWindow`。H.264/H.265 的解码和 MP4 导出过程，CPU 完全不接触像素，全链路在 GPU 显存内高速完成。
*   **RendererFactory (动态渲染探测)**：位于 `rhi/`。启动时动态 `dlopen` 探测 Vulkan 能力，如果失败则无缝降级到 OpenGL ES 3.1/3.2，并解析支持 Compute Shader。

### 2.2 Android 端架构 (Android Integration)
代码位于 `platform/android/`，严格遵循现代化 Android 组件化架构。

*   **JNI 桥接层**：在 `platform/android/jni` 下，将 C++ 的 `std::shared_ptr` 封装并传递给 Kotlin。使用 `<android/native_window_jni.h>` 实现 Surface 的零拷贝绑定。
*   **Kotlin SDK 门面**：在 `platform/android/kotlin` 下，提供对上层极其友好的面向对象 API（如 `VideoPlayer`, `CameraCapture`）。
*   **Jetpack Compose UI (高仿剪映)**：
    *   **模块化拆分**：主 App 壳在 `app/` 模块，剪辑业务逻辑全在 `feature/editor/` 模块。
    *   **核心界面**：
        *   `CameraScreen`：用于全屏相机采集录制。
        *   `EditorScreen`：五段式经典布局（顶栏、预览区、控制栏、时间轴、工具栏）。
    *   **状态与主题**：接入了 Material 3 Design System，支持一键切换 `Light/Dark` (浅色/深色) 主题，全局状态响应。

## 3. 开发规范与要求 (Development Guidelines)
作为 Agent，在处理或修改本项目时，必须严格遵守以下规范：

1.  **C++ 开发原则**：
    *   严禁在多媒体数据链路（例如解码后）引入 CPU 内存拷贝 (避免使用 `glReadPixels` 或提取 YUV 字节数组)，必须保持 GPU 纹理直通 (Zero-Copy)。
    *   严禁在跨线程调度中使用 `while(true) + sleep`，必须使用现有的 `MessageLoop` 提交异步或延时任务。
    *   任何涉及特定平台的 API 调用 (如 Android 的 NDK, Apple 的 VideoToolbox)，必须隔离在 `hal/` 或 `rhi/` 层，通过抽象接口 (如 `IRenderer`, `IDecoderPool`) 暴露。
2.  **JNI 与 Android 开发原则**：
    *   严禁在 JNI 层使 C++ 直接 Crash，必须使用 `JniExceptionHandler` 将 `std::exception` 转换为 Kotlin 的 `VideoSdkError`。
    *   UI 层必须且只能使用 `Jetpack Compose`，禁止回退使用 XML 布局。UI 代码需抽离为独立的 `@Composable` 函数以保持组件化。
3.  **构建系统 (CMake/Gradle)**：
    *   如果要引入 NDK 的新库 (如 libandroid, libmediandk)，必须在 `platform/android/CMakeLists.txt` 或底层对应模块的 CMake 中显示 link，避免在 iOS/Mac 编译时报错。
4.  **注释规范**：
    *   所有的公开类、核心方法、复杂算法（尤其涉及多线程、锁、同步、GPU/CPU 数据交互）必须编写详细的**中文注释**。
