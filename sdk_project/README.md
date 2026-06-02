# Video SDK Project (视频剪辑引擎与演示工程)

## 📖 项目简介 (Introduction)

本项目是一个**工业级、跨平台的音视频剪辑底层引擎**及配套的 Android 演示工程。其底层架构设计深度对标了字节跳动（如剪映、抖音使用的 VESDK）和快手等顶尖大厂的非线性编辑（NLE）技术模型。

引擎核心由 **C++17** 编写，能够在极低的 CPU 和内存占用下，完成极其复杂的视频解码、多轨特效渲染、音视频精准同步以及硬件视频压制导出。

配套的 Android 端 Demo 使用了最新的 **Jetpack Compose** 框架，实现了一个高仿“剪映”的 UI 界面，真正做到了“UI 声明式组件化”与“底层硬核渲染”的完美结合。

---

## ✨ 核心特性 (Key Features)

### 🚀 极致性能 (High Performance)
*   **端到端零拷贝 (Zero-Copy Pipeline)**：在 Android 平台上，借助 NDK `AMediaCodec`，实现了从 H.264/H.265 硬件解码 -> GPU 特效渲染 -> MP4 硬件编码导出的全链路显存直通。CPU **零干预、零拷贝**。
*   **无锁并发音画同步 (Lock-Free A/V Sync)**：摒弃传统 `sleep` 轮询。采用缓存行对齐的 `LockFreeRingBuffer` 配合 `MessageLoop` 事件驱动模型，基于音频主时钟实现毫秒级的视频帧精准调度与休眠。

### 🎨 现代图形架构 (Modern Graphics Architecture)
*   **DAG 渲染管线 (RenderGraph)**：所有的视频源、转场、滤镜都被抽象为可自由拓扑排序的节点。引擎通过统一的 FBO 显存池管理节点间的数据流转。
*   **动态 RHI 后端探测 (Renderer Factory)**：启动时自动 `dlopen` 试探 Vulkan 支持。若不支持则无缝降级至 OpenGL ES 3.1/3.2，并自动探测 `Compute Shader` 计算能力。

### 📱 纯净的 Android 接入体验
*   提供极其干净的 Kotlin Facade API（`VideoPlayer`, `CameraCapture`, `TimelineExporter`）。
*   UI 层面只需传入一个原生的 `android.view.Surface`，底层即刻接管渲染。
*   支持一键切换全局 Light / Dark 主题。

---

## 📚 文档指南 (Documentation)

更详细的架构原理、功能清单和 API 用法，请参阅 `docs/` 目录下的专属文档：

*   [**技术架构设计文档** (`docs/Technical_Architecture_Design.md`)](docs/Technical_Architecture_Design.md)：深入理解 NLE 模型、RenderGraph 和同步引擎原理。
*   [**功能说明文档** (`docs/Functional_Specification.md`)](docs/Functional_Specification.md)：查看目前支持的摄像、预览、剪辑和导出功能清单。
*   [**API 接口参考** (`docs/API_Reference.md`)](docs/API_Reference.md)：供 UI 开发同学调用的 Kotlin / C++ 门面类接口说明。
*   [**Agent 开发规范** (`AGENTS.md`)](AGENTS.md)：对本代码库进行二次开发时必须遵守的 C++ / JNI 规范。

---

## 🛠️ 构建与运行 (Build Instructions)

本项目支持在 Linux / macOS / 环境下进行本地 C++ 引擎编译测试，也支持通过 JNI 打包为 Android `.so` 库。

### 1. 编译纯 C++ 核心层及测试用例 (Linux / macOS)
```bash
cd sdk_project
mkdir build && cd build
cmake ..
make -j4

# 运行渲染管线测试
./core/TestRenderGraph

# 运行无锁音视频同步测试
./media/TestAVSync
```

### 2. 编译 Android JNI 动态链接库 (`libVideoSDKJni.so`)
为了方便在没有完整 Android NDK 环境下验证 JNI 语法，项目内嵌了 `third_party/ndk_mock`。
```bash
cd sdk_project
mkdir build_android_jni && cd build_android_jni
# 指定编译平台目录下的 CMakeLists
cmake ../platform/android
make -j4
```

### 3. 运行 Android Demo (Android Studio)
*   打开 Android Studio。
*   将 `sdk_project/platform/android` 作为一个完整的 Android Project 导入。
*   Sync Gradle 并连接 Android 真机运行。
*   *注意：真机运行需配置真实的 NDK 路径以替换 mock 库。*

---
**License**: 内部开源参考项目
