#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "rhi/interface/IRenderer.h"

namespace video_sdk {
namespace core {

/**
 * @brief 渲染上下文 (Render Context)。
 * 在整个 RenderGraph 的单帧渲染管线中，所有的节点 (RenderNode) 共享此上下文对象。
 * 它包含了调用底层 RHI (如 OpenGL/Vulkan API) 的句柄，以及当前屏幕尺寸和时间戳。
 */
struct RenderContext {
    std::shared_ptr<rhi::IRenderer> renderer;
    int targetWidth;
    int targetHeight;
    int64_t currentPts;
};

/**
 * @brief 渲染管线节点基类 (RenderNode)。
 * 核心原理：
 * 工业级的图像处理不再是一连串硬编码的函数调用，而是基于节点图 (Node Graph)。
 * 所有的操作（视频解码源、美颜滤镜、调色、转场融合）都被抽象为一个继承自此基类的节点。
 * 每个节点负责一件事：“从我的前置节点获取输入的图像数据，使用 Shader 处理后，画到我的专属输出 FBO 上”。
 */
class RenderNode {
public:
    explicit RenderNode(const std::string& name);
    virtual ~RenderNode() = default;

    const std::string& getName() const { return m_name; }

    // --- 图拓扑连接接口 (Graph Topology) ---

    /**
     * @brief 添加前置依赖节点 (即本节点的上游输入)。
     * 例如，美颜节点的 input 是视频源节点；而叠图转场的 input 则是两段不同的视频源节点。
     */
    void addInputNode(std::shared_ptr<RenderNode> node);
    const std::vector<std::shared_ptr<RenderNode>>& getInputNodes() const { return m_inputs; }

    /**
     * @brief 设置该节点本次渲染输出的目标显存 (FBO)。
     * 引擎在运行时会通过对象池动态借给该节点一块 FBO，节点处理完的画面必须保存在这里。
     */
    void setOutputFbo(rhi::FrameBufferObject* fbo) { m_outputFbo = fbo; }
    rhi::FrameBufferObject* getOutputFbo() const { return m_outputFbo; }

    // --- 核心渲染接口 (Rendering Interface) ---

    // 初始化 Shader、网格和 VBO 资源 (仅在管线初次搭建时调用)
    virtual void prepare(RenderContext& context) {}

    /**
     * @brief 执行当前节点的计算与渲染核心操作 (DrawCall)。
     * 【前提保证】：引擎底层使用拓扑排序算法，保证在调用本节点的 `process` 时，
     * 所有挂在 `getInputNodes()` 里的前置节点都已经执行完毕，且它们的 `m_outputFbo` 中已经有了最新的画面纹理。
     */
    virtual void process(RenderContext& context) = 0;

    // 释放占用的 GPU 资源
    virtual void release(RenderContext& context) {}

protected:
    std::string m_name;
    std::vector<std::shared_ptr<RenderNode>> m_inputs; // 上游依赖列表
    rhi::FrameBufferObject* m_outputFbo = nullptr;     // 存放当前节点计算产物的地方
};

// 以下是几个经典内置节点的定义：

/**
 * @brief 视频源节点 (Source Node)。
 * 起点节点，它没有前置输入。它的任务是将 Android MediaCodec 解码到 SurfaceTexture 的
 * OES 外部纹理或者 FFmpeg 解出来的 YUV 数据，通过基础 Shader 画到本节点的 FBO 上，
 * 供后续的滤镜消费。
 */
class SourceNode : public RenderNode {
public:
    explicit SourceNode(const std::string& name, uint32_t externalTextureId);
    void process(RenderContext& context) override;
private:
    uint32_t m_externalTextureId;
};

/**
 * @brief 基础滤镜节点 (Filter Node，如磨皮、LUT调色)。
 * 标准的 1进1出 节点：接收 1 个前置节点的 FBO 纹理作为输入，经过 GLSL/SPIR-V 处理，产生 1 个结果 FBO 输出。
 */
class FilterNode : public RenderNode {
public:
    explicit FilterNode(const std::string& name, const std::string& filterType);
    void process(RenderContext& context) override;
private:
    std::string m_filterType;
};

/**
 * @brief 转场/融合节点 (Transition Node)。
 * 多进单出 节点：接收 2 个前置视频节点的输入，根据 progress 进度（如 0.5），
 * 在 Shader 中混合两个画面的像素 (如 Crossfade 交叉溶解)，产生 1 个混合后的 FBO 输出。
 */
class TransitionNode : public RenderNode {
public:
    explicit TransitionNode(const std::string& name, float progress);
    void process(RenderContext& context) override;
private:
    float m_progress; // 转场进度百分比 0.0 ~ 1.0
};

} // namespace core
} // namespace video_sdk
