#pragma once

#include <string>
#include <vector>
#include <memory>
#include "rhi/interface/IRenderer.h"

namespace video_sdk {
namespace core {

/**
 * @brief 渲染上下文。
 * 使用纯正的 RHI ITexture 代替暴露底层细节的 FrameBufferObject。
 */
struct RenderContext {
    std::shared_ptr<rhi::IRenderer> renderer;
    int targetWidth;
    int targetHeight;
    int64_t currentPts;
};

/**
 * @brief 渲染管线节点基类。
 */
class RenderNode {
public:
    explicit RenderNode(const std::string& name);
    virtual ~RenderNode() = default;

    const std::string& getName() const { return m_name; }

    void addInputNode(std::shared_ptr<RenderNode> node);
    const std::vector<std::shared_ptr<RenderNode>>& getInputNodes() const { return m_inputs; }

    // 使用 RHI Texture 作为输出目标
    void setOutputTexture(std::shared_ptr<rhi::ITexture> texture) { m_outputTexture = texture; }
    std::shared_ptr<rhi::ITexture> getOutputTexture() const { return m_outputTexture; }

    virtual void prepare(RenderContext& context) {}
    virtual void process(RenderContext& context) = 0;
    virtual void release(RenderContext& context) {}

protected:
    std::string m_name;
    std::vector<std::shared_ptr<RenderNode>> m_inputs;
    std::shared_ptr<rhi::ITexture> m_outputTexture;
};

// ... SourceNode, FilterNode, TransitionNode ...
class SourceNode : public RenderNode {
public:
    explicit SourceNode(const std::string& name, uint32_t externalTextureId);
    void process(RenderContext& context) override;
private:
    uint32_t m_externalTextureId;
};

class FilterNode : public RenderNode {
public:
    explicit FilterNode(const std::string& name, const std::string& filterType);
    void process(RenderContext& context) override;
private:
    std::string m_filterType;
};

class TransitionNode : public RenderNode {
public:
    explicit TransitionNode(const std::string& name, float progress);
    void process(RenderContext& context) override;
private:
    float m_progress;
};

} // namespace core
} // namespace video_sdk
