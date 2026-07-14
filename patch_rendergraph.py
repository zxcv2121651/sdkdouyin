import re

with open('core/engine/RenderGraph.cpp', 'r') as f:
    content = f.read()

# Replace the block inside render()
old_block = """    for (auto& node : m_executionSequence) {
        // 工业级 FBO 显存池化机制：
        // 不允许每个特效节点内部自己 `glGenTextures`。
        // 而是在执行前，统一向 RHI (Renderer) 申请一块可用的 FBO 显存作为当前节点的输出目标。

        static uint32_t pseudoFboCounter = 1;
        auto fbo = new rhi::FrameBufferObject{pseudoFboCounter++, pseudoFboCounter * 10, context.targetWidth, context.targetHeight};

        activeFbos.push_back(fbo);
        node->setOutputFbo(fbo);

        // 触发节点内部使用 OpenGL ES / Vulkan API 进行真实的 DrawCall
        node->process(context);
    }

    // 渲染结束，清理阶段：将所有 FBO 放回对象池以供下一帧复用，避免显存碎片化
    for (auto fbo : activeFbos) {
        // 实际调用：context.renderer->releaseFBO(fbo);
        delete fbo; // 伪实现直接 delete
    }"""

new_block = """    for (auto& node : m_executionSequence) {
        // 从全局的 FBOPool 申请显存资源，避免高频显存分配导致的系统卡顿
        rhi::FrameBufferObject* fbo = nullptr;
        if (context.fboPool) {
            fbo = context.fboPool->acquireFBO(context.targetWidth, context.targetHeight);
        } else {
            // Fallback for tests if pool is not injected
            static uint32_t pseudoFboCounter = 1;
            fbo = new rhi::FrameBufferObject{pseudoFboCounter++, pseudoFboCounter * 10, context.targetWidth, context.targetHeight};
        }

        activeFbos.push_back(fbo);
        node->setOutputFbo(fbo);

        // 触发节点内部进行真实的 DrawCall
        node->process(context);
    }

    // 渲染结束，清理阶段：将所有 FBO 放回对象池以供下一帧复用，避免显存碎片化
    for (auto fbo : activeFbos) {
        if (context.fboPool) {
            context.fboPool->releaseFBO(fbo);
        } else {
            delete fbo;
        }
    }"""

if old_block in content:
    content = content.replace(old_block, new_block)
    with open('core/engine/RenderGraph.cpp', 'w') as f:
        f.write(content)
    print("RenderGraph.cpp updated successfully.")
else:
    print("Error: Could not find the old block to replace.")
