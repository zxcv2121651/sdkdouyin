
# SDK 目录结构规范性分析与重构建议

您观察得非常敏锐。当前的 `sdk_project` 虽然在**代码架构分层**（逻辑上的解耦）上做得很优秀（分为 core, rhi, media 等），但在**物理目录结构**（文件系统的组织）上，确实不符合现代标准 C++ 开源/商业 SDK 的规范。

以下是对其不标准之处的深度分析以及标准改造方案。

## 1. 当前目录结构的四大 "不标准" 之处

### 1.1 公开接口与私有实现严重混杂 (缺少 `include/` 目录)
*   **现状：** 在当前结构中，像 `core/engine/RenderGraph.h` 和 `core/engine/RenderGraph.cpp` 放在同一个目录下。
*   **问题：** 作为一个 SDK，你需要对外提供 Header（头文件）供外部集成方调用，同时隐藏所有的实现细节（私有 Header 和 CPP 文件）。目前这种结构在打包发布 SDK 时，很难使用脚本干净地抽离出 "对外公开 API 头文件"。所有的内部类头文件全都暴露在外。

### 1.2 测试用例与生产代码耦合 (缺少 `tests/` 目录)
*   **现状：** 我们之前运行的测试代码 `core/engine/TestRenderGraph.cpp` 和 `media/sync/TestAVSync.cpp` 直接硬塞在了核心业务逻辑的文件夹里。
*   **问题：** 这极大地污染了生产级源码树。测试不应该成为最终库 `libVideoCore.a` 的一部分，这会增加构建的混乱程度，也不利于 CI/CD 集中管理测试集（如 GoogleTest 框架的接入）。

### 1.3 示例工程未独立抽离 (缺少 `examples/` 或 `samples/` 目录)
*   **现状：** `platform/android/app`（一个完整的 Android Demo 工程）和 `platform/android/jni`（SDK 的 JNI 胶水层）被混在一起放在了 `platform/` 下。
*   **问题：** SDK 的源码库应当保持纯粹。Demo App 通常只是依赖 SDK 产物（aar 或 so+头文件）的独立工程。将 Demo 代码和 SDK 底层源码混在一个目录下，对开发者理解 SDK 边界非常不友好。

### 1.4 缺乏构建脚本与文档的收敛 (缺少 `scripts/` 和 `docs/`)
*   **现状：** 没有专门存放 Doxygen API 文档的目录，也没有看到专门针对 iOS/Android 跨平台交叉编译的 Shell 或 Python 打包脚本。

---

## 2. 业界标准 C++ SDK 目录结构改造方案

参考 Google、字节跳动、腾讯等大厂的 C++ 底层库（如 WebRTC, ijkplayer, filament），一个工业级多媒体 SDK 的标准物理目录应该重构成如下结构：

```text
sdk_project_standard/
├── CMakeLists.txt              # 顶层 CMake，提供控制开关 (如 -DBUILD_TESTS=ON)
├── README.md
├── include/                    # 【核心改变】对外公开的 API 头文件
│   └── video_sdk/              # 使用命名空间作为文件夹名防止冲突
│       ├── core/               # 例如: Timeline.h, Clip.h
│       ├── modules/            # 例如: VideoEditor.h, VideoPlayer.h
│       └── VideoSdkError.h     # 公开的枚举和错误码
├── src/                        # 【核心改变】所有私有实现和内部头文件
│   ├── core/                   # RenderGraph.cpp, RenderNode.cpp, RenderNode.h(私有)
│   ├── rhi/                    # VulkanRenderer.cpp, GLESRenderer.cpp
│   ├── hal/
│   ├── media/
│   └── modules/
├── platform/                   # 各平台的胶水层接口 (JNI / ObjC++)
│   ├── android/                # 仅包含 jni/ 和 kotlin/ sdk 封装代码
│   └── ios/                    # 仅包含 objc/ 和 swift/ sdk 封装代码
├── third_party/                # 依赖的外部库 (ffmpeg, vulkan-headers, googletest)
├── tests/                      # 【核心改变】所有的单元测试和集成测试
│   ├── core_tests/             # 原来的 TestRenderGraph.cpp 移到这里
│   └── media_tests/            # 原来的 TestAVSync.cpp 移到这里
├── examples/                   # 【核心改变】独立出来的示例工程
│   ├── android_demo/           # 原来的 platform/android/app
│   └── ios_demo/
├── docs/                       # API 文档和架构设计图
└── scripts/                    # 跨平台构建、打包、发布脚本 (如 build_android.sh)
```

## 3. 为什么标准结构长这样？(收益分析)

1. **绝对的 API 边界安全：** 打包 SDK 时，打包脚本只需无脑 Copy `include/` 目录即可，绝不可能不小心把 `VulkanRenderer.h` 这种极度底层的私有头文件泄露给 SDK 的使用方。
2. **极简的 CMake 依赖管理：** 在 `CMakeLists.txt` 中，只需声明 `target_include_directories(VideoSDK PUBLIC include)`。所有内部依赖可以通过 `PRIVATE src/` 隔离。
3. **清晰的模块职责：** 新加入的工程师立刻就能分清：要了解 SDK 怎么用，看 `examples/` 和 `include/`；要改 Bug，进 `src/`；要加功能，别忘了在 `tests/` 里写测试。

## 结论

当前代码库在 **C++ 软件架构模式** 上达到了很高的水平，但在 **工程化物理目录管理** 上仍处于 "作坊期"（所有文件按功能平铺混杂）。如果要将其真正作为商业化或大型开源项目发布，必须进行上述的 `include/` 与 `src/` 分离、测试与示例独立等标准化重构。
