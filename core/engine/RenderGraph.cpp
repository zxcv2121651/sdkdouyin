#include "RenderGraph.h"
#include <iostream>
#include <queue>
#include <algorithm>

namespace video_sdk {
namespace core {

RenderGraph::RenderGraph() {}

RenderGraph::~RenderGraph() {}

void RenderGraph::addNode(std::shared_ptr<RenderNode> node) {
    if (std::find(m_nodes.begin(), m_nodes.end(), node) == m_nodes.end()) {
        m_nodes.push_back(node);
    }
}

void RenderGraph::setOutputNode(std::shared_ptr<RenderNode> node) {
    m_outputNode = node;
    addNode(node);
}

bool RenderGraph::compile() {
    if (!m_outputNode) {
        std::cerr << "[RenderGraph] Compile failed: Output node not set." << std::endl;
        return false;
    }
    return topologicalSort();
}

bool RenderGraph::topologicalSort() {
    m_executionSequence.clear();

    std::unordered_map<std::shared_ptr<RenderNode>, int> inDegree;
    std::unordered_map<std::shared_ptr<RenderNode>, std::vector<std::shared_ptr<RenderNode>>> adjList;

    for (auto& node : m_nodes) {
        inDegree[node] = 0;
    }

    for (auto& node : m_nodes) {
        for (auto& input : node->getInputNodes()) {
            adjList[input].push_back(node);
            inDegree[node]++;
        }
    }

    std::queue<std::shared_ptr<RenderNode>> q;
    for (auto& pair : inDegree) {
        if (pair.second == 0) {
            q.push(pair.first);
        }
    }

    while (!q.empty()) {
        auto current = q.front();
        q.pop();
        m_executionSequence.push_back(current);

        for (auto& neighbor : adjList[current]) {
            inDegree[neighbor]--;
            if (inDegree[neighbor] == 0) {
                q.push(neighbor);
            }
        }
    }

    if (m_executionSequence.size() != m_nodes.size()) {
        std::cerr << "[RenderGraph] Compile failed: Cycle detected!" << std::endl;
        return false;
    }

    std::cout << "[RenderGraph] Compiled successfully. Sequence: ";
    for (size_t i = 0; i < m_executionSequence.size(); ++i) {
        std::cout << m_executionSequence[i]->getName() << (i == m_executionSequence.size() - 1 ? "" : " -> ");
    }
    std::cout << " -> END" << std::endl;

    return true;
}

void RenderGraph::render(RenderContext& context) {
    if (m_executionSequence.empty()) return;

    std::cout << "=== [RenderGraph] Starting Frame Rendering ===" << std::endl;

    std::vector<rhi::FrameBufferObject*> activeFbos;

    uint32_t pseudoFboCounter = 1;

    for (auto& node : m_executionSequence) {
        rhi::FrameBufferObject* fbo = nullptr;
        if (context.fboPool) {
            fbo = context.fboPool->acquireFBO(context.targetWidth, context.targetHeight);
        } else if (context.renderer) {
            fbo = context.renderer->acquireFBO(context.targetWidth, context.targetHeight);
        } else {
            fbo = new rhi::FrameBufferObject{pseudoFboCounter++, pseudoFboCounter * 10, context.targetWidth, context.targetHeight};
        }
        activeFbos.push_back(fbo);
        node->setOutputFbo(fbo);
    }

    for (auto& node : m_executionSequence) {
        node->process(context);
    }

    for (auto fbo : activeFbos) {
        if (context.fboPool) {
            context.fboPool->recycleFBO(fbo);
        } else if (context.renderer) {
            context.renderer->releaseFBO(fbo);
        } else {
            delete fbo;
        }
    }
    activeFbos.clear();

    std::cout << "=== [RenderGraph] Frame Rendering Completed ===\n" << std::endl;
}

}
}
