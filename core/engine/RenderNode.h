#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "rhi/interface/IRenderer.h"
#include "core/include/IFBOPool.h"

namespace video_sdk {
namespace core {

/**
 * @brief 渲染上下文。
 * 用于在 RenderGraph 的各个节点之间传递状态、当前的 FBO 等。
 */
struct RenderContext {
    std::shared_ptr<rhi::IRenderer> renderer;
    std::shared_ptr<IFBOPool> fboPool;
    int targetWidth;
    int targetHeight;
    int64_t currentPts;
};

/**
 * @brief 渲染管线节点基类。
 * 所有视频处理（源、滤镜、转场）都继承自此节点。
 */
class RenderNode {
public:
    explicit RenderNode(const std::string& name);
    virtual ~RenderNode() = default;

    const std::string& getName() const { return m_name; }

    // --- 图拓扑连接接口 ---

    // 添加前置依赖节点 (本节点的输入)
    void addInputNode(std::shared_ptr<RenderNode> node);
    const std::vector<std::shared_ptr<RenderNode>>& getInputNodes() const { return m_inputs; }

    // 设置该节点的输出结果纹理/FBO
    void setOutputFbo(rhi::FrameBufferObject* fbo) { m_outputFbo = fbo; }
    rhi::FrameBufferObject* getOutputFbo() const { return m_outputFbo; }

    // --- 核心渲染接口 ---

    // 初始化资源
    virtual void prepare(RenderContext& context) {}

    // 执行当前节点的渲染操作
    // 前提：系统调度保证在调用 process 时，所有的 inputNodes 已经 process 完毕，
    // 它们的 m_outputFbo 已经包含了前置步骤的结果。
    virtual void process(RenderContext& context) = 0;

    // 释放资源
    virtual void release(RenderContext& context) {}

protected:
    std::string m_name;
    std::vector<std::shared_ptr<RenderNode>> m_inputs;
    rhi::FrameBufferObject* m_outputFbo = nullptr; // 该节点渲染的最终产物
};

/**
 * @brief 视频源节点。
 * 不依赖任何前置输入，负责把 OES 或 YUV 绘制到自己的输出 FBO 上。
 */
class SourceNode : public RenderNode {
public:
    explicit SourceNode(const std::string& name, uint32_t externalTextureId);
    void process(RenderContext& context) override;
private:
    uint32_t m_externalTextureId;
};

/**
 * @brief 滤镜节点 (如磨皮、LUT)。
 * 接收 1 个输入，产生 1 个输出。
 */
class FilterNode : public RenderNode {
public:
    explicit FilterNode(const std::string& name, const std::string& filterType);
    void process(RenderContext& context) override;
private:
    std::string m_filterType;
};

/**
 * @brief 转场节点 (如两段视频中间的 Crossfade)。
 * 接收 2 个输入，产生 1 个输出。
 */
class TransitionNode : public RenderNode {
public:
    explicit TransitionNode(const std::string& name, float progress);
    void process(RenderContext& context) override;
private:
    float m_progress; // 转场进度 0.0 ~ 1.0
};

} // namespace core
} // namespace video_sdk
