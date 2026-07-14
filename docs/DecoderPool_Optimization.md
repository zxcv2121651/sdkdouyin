# DecoderPool 商业级优化报告

## 优化背景
在移动端多轨道视频编辑（如剪映、VESDK）中，硬件解码器（`MediaCodec` 等）的实例化通常极其耗时（10ms~50ms不等），且系统存在严格的最大并发数量限制（通常为 16 个实例）。频繁地创建和销毁会导致严重的卡顿和系统 OOM。之前的实现仅有基础的 LRU，存在“正在使用的解码器可能被误淘汰”和“初始化阻塞主渲染线程”的致命缺陷。

## 核心优化点

### 1. 状态锁定机制 (Pin/Unpin)
在多轨剪辑中，渲染管线正在使用的解码器绝对不能被 LRU 机制淘汰。
- 引入了 `pinDecoder()` 和 `unpinDecoder()` 接口。
- 修改了淘汰策略 `evictOneUnpinned_Locked()`：当达到 `maxCapacity` 时，引擎会从 LRU 的尾部向前扫描，仅淘汰那些处于 **Unpinned (未锁定/空闲)** 状态的解码器。
- 这从根本上防止了“正在渲染的画面突然黑屏”的致命 Bug。

### 2. 异步预读机制 (Async Pre-fetching)
- 引入了 `preloadHardwareDecoderAsync()` 接口。
- 该接口在底层新开线程（商业化中常结合 IO `MessageLoop`）异步执行 `newDecoder->initialize(...)`。
- 上层可以根据 NLE（非线性编辑）Timeline 模型，提前 2~3 秒将即将进入播放区间的轨道素材传入，在后台完成极其耗时的硬件解码器创建和内存分配。当播放头真正到达时，主渲染线程调用 `requestHardwareDecoderInstance` 可以做到 **0 延迟命中缓存**，保证极度流畅的播放体验。

### 3. 内存压力响应 (Memory Pressure Handling)
- 引入了 `evictAllUnpinned()`。
- 当 Android App 收到系统的 `onTrimMemory(TRIM_MEMORY_RUNNING_CRITICAL)` 等回调时，可以通过该接口瞬间释放所有处于休眠（Unpinned）状态的硬件解码器实例，主动归还显存和系统硬件资源，防止被系统强杀 (OOM Killer)。

## 总结
通过引入 Pin 状态、异步预读与内存告警响应，目前的 `hal::DecoderPool` 已经具备了顶级移动端视频编辑引擎资源管理的雏形，有效解决了性能卡顿和稳定性问题。
