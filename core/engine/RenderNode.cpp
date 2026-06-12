#include "RenderNode.h"
#include <iostream>

namespace video_sdk {
namespace core {

RenderNode::RenderNode(const std::string& name) : m_name(name) {}

void RenderNode::addInputNode(std::shared_ptr<RenderNode> node) {
    if (node) {
        m_inputs.push_back(node);
    }
}

// 模拟的 SourceNode
SourceNode::SourceNode(const std::string& name, uint32_t externalTextureId)
    : RenderNode(name), m_externalTextureId(externalTextureId) {}

void SourceNode::process(RenderContext& context) {
    if (!m_outputTexture) return;
    std::cout << "[RenderNode] Executing [" << m_name << "] -> Drawing to texture " << m_outputTexture->getNativeId() << std::endl;
}

// 模拟的 FilterNode
FilterNode::FilterNode(const std::string& name, const std::string& filterType)
    : RenderNode(name), m_filterType(filterType) {}

void FilterNode::process(RenderContext& context) {
    if (!m_outputTexture || m_inputs.empty() || !m_inputs[0]->getOutputTexture()) return;
    std::cout << "[RenderNode] Executing [" << m_name << " (" << m_filterType << ")] -> Reading from texture "
              << m_inputs[0]->getOutputTexture()->getNativeId() << ", Writing to texture " << m_outputTexture->getNativeId() << std::endl;
}

// 模拟的 TransitionNode
TransitionNode::TransitionNode(const std::string& name, float progress)
    : RenderNode(name), m_progress(progress) {}

void TransitionNode::process(RenderContext& context) {
    if (!m_outputTexture || m_inputs.size() < 2 || !m_inputs[0]->getOutputTexture() || !m_inputs[1]->getOutputTexture()) return;
    std::cout << "[RenderNode] Executing [" << m_name << " (Transition progress " << m_progress << ")] -> Mixing texture "
              << m_inputs[0]->getOutputTexture()->getNativeId() << " and texture " << m_inputs[1]->getOutputTexture()->getNativeId()
              << ", Writing to texture " << m_outputTexture->getNativeId() << std::endl;
}

} // namespace core
} // namespace video_sdk
