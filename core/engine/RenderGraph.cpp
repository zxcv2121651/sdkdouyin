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

    // 1. 统计每个节点的入度 (在这个上下文中，"输入节点" 意味着依赖项)
    // 注意：Graph 的流向是 InputNode -> CurrentNode。
    // 所以 CurrentNode 依赖 InputNode 先执行完毕。
    // 因此在拓扑排序中，InputNode 到 CurrentNode 有一条边。

    std::unordered_map<std::shared_ptr<RenderNode>, int> inDegree;
    std::unordered_map<std::shared_ptr<RenderNode>, std::vector<std::shared_ptr<RenderNode>>> adjList;

    for (auto& node : m_nodes) {
        inDegree[node] = 0; // 初始化
    }

    for (auto& node : m_nodes) {
        for (auto& input : node->getInputNodes()) {
            // input 必须在 node 之前执行
            adjList[input].push_back(node);
            inDegree[node]++;
        }
    }

    // 2. 将入度为 0 的节点加入队列 (通常是 SourceNode)
    std::queue<std::shared_ptr<RenderNode>> q;
    for (auto& pair : inDegree) {
        if (pair.second == 0) {
            q.push(pair.first);
        }
    }

    // 3. 开始执行 Kahn 算法
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

    // 真正的 RHI FBO 分配池
    // 在这里我们为了兼容测试，临时模拟使用 Renderer 提供的 createTexture2D
    std::vector<std::shared_ptr<rhi::ITexture>> activeTextures;

    // 1. 为每个将要执行的 Node 分配一个临时的输出 Texture
    for (auto& node : m_executionSequence) {
        if (context.renderer) {
            auto tex = context.renderer->createTexture2D(context.targetWidth, context.targetHeight, rhi::TextureFormat::RGBA8);
            activeTextures.push_back(tex);
            node->setOutputTexture(tex);
        } else {
            // For pure mockup where renderer is null, do nothing or handle differently
            // Actually the TestRenderGraph manually injects MockTextures
        }
    }

    // 2. 按顺序执行
    for (auto& node : m_executionSequence) {
        node->process(context);
    }

    // 3. 回收临时的 FBO Texture
    activeTextures.clear(); // shared_ptr 自动释放
}

} // namespace core
} // namespace video_sdk
