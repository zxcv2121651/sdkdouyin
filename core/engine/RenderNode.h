#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "rhi/interface/IRenderer.h"
#include "rhi/DefaultFBOPool.h"

namespace video_sdk {
namespace core {

struct RenderContext {
    std::shared_ptr<rhi::IRenderer> renderer;
    std::shared_ptr<rhi::IFBOPool> fboPool;
    int targetWidth;
    int targetHeight;
    int64_t currentPts;
};

class RenderNode {
public:
    explicit RenderNode(const std::string& name);
    virtual ~RenderNode() = default;

    const std::string& getName() const { return m_name; }

    void addInputNode(std::shared_ptr<RenderNode> node);
    const std::vector<std::shared_ptr<RenderNode>>& getInputNodes() const { return m_inputs; }

    void setOutputFbo(rhi::FrameBufferObject* fbo) { m_outputFbo = fbo; }
    rhi::FrameBufferObject* getOutputFbo() const { return m_outputFbo; }

    virtual void prepare(RenderContext& context) {}
    virtual void process(RenderContext& context) = 0;
    virtual void release(RenderContext& context) {}

protected:
    std::string m_name;
    std::vector<std::shared_ptr<RenderNode>> m_inputs;
    rhi::FrameBufferObject* m_outputFbo = nullptr;
};

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

}
}
