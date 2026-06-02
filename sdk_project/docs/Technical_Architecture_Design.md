# 视频剪辑 SDK 技术架构设计文档

## 1. 整体架构思想 (Architecture Philosophy)
本 SDK 采用**高内聚、低耦合**的跨平台架构设计，对标业界顶级的短视频底层引擎（如字节跳动 VESDK、快手等）。
架构分为四大层：
1.  **业务门面层 (Facade Layer)**：Android/iOS 的 UI 代码不直接接触底层，而是通过 Kotlin/Swift 封装的 SDK 门面类与 C++ 交互。
2.  **核心控制层 (Core / Modules)**：NLE（非线性编辑）数据模型、特效管家。
3.  **渲染与同步层 (RHI / Media)**：跨平台的图形渲染接口和基于音频主时钟的无锁同步引擎。
4.  **硬件抽象层 (HAL)**：屏蔽不同操作系统底层的硬件编解码器（如 Android MediaCodec、iOS VideoToolbox）。

---

## 2. 核心技术模块解析

### 2.1 非线性编辑模型 (NLE - Non-Linear Editing)
*   **位置**：`core/timeline/`
*   **原理**：采用纯数据驱动。`Timeline` 是一个全局状态机，内部包含多条轨道（Track），轨道上分布着多个媒体片段（`Clip`）。
*   **时间映射**：`Clip` 维护了 `Source Time`（源文件时间）和 `Timeline Time`（全局时间）的两套坐标系，支持复杂的视频裁剪、变速、倒放等操作，而不需要对原视频文件做任何物理修改。

### 2.2 DAG 有向无环图渲染引擎 (RenderGraph)
*   **位置**：`core/engine/`
*   **原理**：摒弃了传统线性的 OpenGL 函数调用。所有的画面源、滤镜、转场都被抽象为 `RenderNode`。
*   **拓扑排序**：引擎使用 Kahn 算法对节点进行拓扑排序，生成展平的执行队列，支持极其复杂的特效网络（如多个画中画加上全局滤镜）。
*   **显存池化**：节点之间通过 FBO (Frame Buffer Object) 传递纹理，系统统一管理 FBO 池，避免了显存碎片的产生。

### 2.3 零功耗音画同步 (Lock-Free A/V Sync)
*   **位置**：`media/sync/`
*   **痛点**：传统的音视频同步往往使用 `sleep()` 阻塞线程，不仅极其耗电，而且误差大导致掉帧。
*   **方案**：
    *   **Master Clock**：使用 `AVSyncClock` 锚定绝对音频时间。
    *   **消息驱动**：引入 `MessageLoop`（基于 `std::condition_variable`），视频帧早了就使用 `postDelayedTask` 挂起（CPU 零消耗），晚了就直接 Drop（丢弃）。
    *   **缓存行对齐**：使用自定义的 `LockFreeRingBuffer` 传递帧数据，加入了 `CACHE_LINE_SIZE` 对齐填充，彻底消除了多核 CPU 下的伪共享（False Sharing）问题。

### 2.4 端到端零拷贝管线 (Zero-Copy Pipeline)
*   **位置**：`hal/codec/`
*   **原理**：拒绝 CPU 参与像素搬运。
    *   **解码**：`AndroidMediaCodecDecoder` 绑定 `Surface`，硬件解码器直接将 H.264/H.265 画面推送到显存变为 OES 纹理。
    *   **渲染**：GPU 内部处理所有节点特效。
    *   **编码**：通过 `AMediaCodec_createInputSurface` 获取表面，GPU 直接将最终画面画给硬件编码器。

### 2.5 动态 RHI 后端 (Renderer Factory)
*   **位置**：`rhi/`
*   **原理**：为了最大化榨干手机硬件性能且保证不崩溃：
    *   启动时使用 `dlopen` 动态试探是否支持 **Vulkan**，支持则启用 Vulkan（预编译 PSO 提升 30% 性能）。
    *   不支持则退化到 **OpenGL ES 3.2/3.1**，通过解析 `GL_VERSION` 动态开启 Compute Shader（计算着色器）支持。
