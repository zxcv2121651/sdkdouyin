#include "RenderNode.h"
#include <iostream>

namespace video_sdk {
namespace core {

RenderNode::RenderNode(const std::string& name) : m_name(name) {}

void RenderNode::addInputNode(std::shared_ptr<RenderNode> node) {
    m_inputs.push_back(node);
}

// ---------------- SourceNode ----------------

SourceNode::SourceNode(const std::string& name, uint32_t externalTextureId)
    : RenderNode(name), m_externalTextureId(externalTextureId) {}

void SourceNode::process(RenderContext& context) {
    if (!m_outputFbo) return;
    std::cout << "[RenderNode] Executing [" << m_name << "] -> Drawing external texture "
              << m_externalTextureId << " to FBO " << m_outputFbo->fboId << std::endl;
    // 实际逻辑：
    // context.renderer->bindFbo(m_outputFbo);
    // draw external texture OesToRgbFilter ...
}

// ---------------- FilterNode ----------------

FilterNode::FilterNode(const std::string& name, const std::string& filterType)
    : RenderNode(name), m_filterType(filterType) {}

void FilterNode::process(RenderContext& context) {
    if (!m_outputFbo || m_inputs.empty()) return;
    auto inputFbo = m_inputs[0]->getOutputFbo();
    if (!inputFbo) return;

    std::cout << "[RenderNode] Executing [" << m_name << " (" << m_filterType << ")] -> Reading from FBO "
              << inputFbo->fboId << ", Writing to FBO " << m_outputFbo->fboId << std::endl;

    // 实际逻辑：
    // context.renderer->bindFbo(m_outputFbo);
    // bindTexture(inputFbo->textureId);
    // draw with specific shader ...
}

// ---------------- TransitionNode ----------------

TransitionNode::TransitionNode(const std::string& name, float progress)
    : RenderNode(name), m_progress(progress) {}

void TransitionNode::process(RenderContext& context) {
    if (!m_outputFbo || m_inputs.size() < 2) return;

    auto source1Fbo = m_inputs[0]->getOutputFbo();
    auto source2Fbo = m_inputs[1]->getOutputFbo();

    if (!source1Fbo || !source2Fbo) return;

    std::cout << "[RenderNode] Executing [" << m_name << " (Transition progress " << m_progress << ")]"
              << " -> Mixing FBO " << source1Fbo->fboId << " and FBO " << source2Fbo->fboId
              << ", Writing to FBO " << m_outputFbo->fboId << std::endl;

    // 实际逻辑：
    // context.renderer->bindFbo(m_outputFbo);
    // glActiveTexture(GL_TEXTURE0); bind(source1Fbo->textureId);
    // glActiveTexture(GL_TEXTURE1); bind(source2Fbo->textureId);
    // glUniform1f(progressLoc, m_progress);
    // draw mix shader ...
}

} // namespace core
} // namespace video_sdk
