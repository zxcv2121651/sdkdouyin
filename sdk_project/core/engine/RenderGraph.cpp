#include "RenderGraph.h"
#include <queue>
#include <iostream>

namespace video_sdk {
namespace core {

RenderGraph::RenderGraph() {}

RenderGraph::~RenderGraph() {}

void RenderGraph::addNode(std::shared_ptr<RenderNode> node) {
    if (node) {
        m_nodes.push_back(node);
    }
}

void RenderGraph::setOutputNode(std::shared_ptr<RenderNode> node) {
    m_outputNode = node;
}

bool RenderGraph::compile() {
    if (m_nodes.empty() || !m_outputNode) return false;
    return topologicalSort();
}

bool RenderGraph::topologicalSort() {
    m_executionSequence.clear();

    // 计算入度 (In-Degree) 和邻接表 (Adjacency List)
    std::unordered_map<std::shared_ptr<RenderNode>, int> inDegree;
    std::unordered_map<std::shared_ptr<RenderNode>, std::vector<std::shared_ptr<RenderNode>>> adjList;

    // 初始化
    for (auto& node : m_nodes) {
        inDegree[node] = 0;
    }

    // 构建图结构: 依赖关系是 input -> current
    for (auto& node : m_nodes) {
        for (auto& input : node->getInputNodes()) {
            adjList[input].push_back(node);
            inDegree[node]++;
        }
    }

    // 寻找所有入度为 0 的节点 (即起点, 通常是 SourceNode)
    std::queue<std::shared_ptr<RenderNode>> zeroInDegreeQueue;
    for (auto& pair : inDegree) {
        if (pair.second == 0) {
            zeroInDegreeQueue.push(pair.first);
        }
    }

    // Kahn's Algorithm
    while (!zeroInDegreeQueue.empty()) {
        auto current = zeroInDegreeQueue.front();
        zeroInDegreeQueue.pop();

        m_executionSequence.push_back(current);

        for (auto& neighbor : adjList[current]) {
            inDegree[neighbor]--;
            if (inDegree[neighbor] == 0) {
                zeroInDegreeQueue.push(neighbor);
            }
        }
    }

    // 检查是否有环
    if (m_executionSequence.size() != m_nodes.size()) {
        std::cerr << "[RenderGraph] ERROR: Cyclic dependency detected!" << std::endl;
        return false;
    }

    std::cout << "[RenderGraph] Compiled successfully. Sequence: ";
    for (auto& n : m_executionSequence) std::cout << n->getName() << " -> ";
    std::cout << "END" << std::endl;

    return true;
}

void RenderGraph::render(RenderContext& context) {
    if (m_executionSequence.empty()) return;

    std::cout << "\n=== [RenderGraph] Starting Frame Rendering ===" << std::endl;

    // 用于记录本帧申请的 FBO，渲染结束后统一回收
    std::vector<rhi::FrameBufferObject*> activeFbos;

    for (auto& node : m_executionSequence) {
        // 工业级 FBO 池化流转：
        // 在实际执行前，动态向 RHI 层申请 FBO 给节点作为输出缓存
        // 如果是最后输出节点，可以直接渲染到屏幕(FBO=0)，这里为了演示都分配离屏FBO

        // 模拟 FBO 申请：假设 Renderer 返回了一个伪 FBO (fboId 递增方便日志观察)
        // 实际调用：auto fbo = context.renderer->acquireFBO(context.targetWidth, context.targetHeight);
        static uint32_t pseudoFboCounter = 1;
        auto fbo = new rhi::FrameBufferObject{pseudoFboCounter++, pseudoFboCounter * 10, context.targetWidth, context.targetHeight};

        activeFbos.push_back(fbo);
        node->setOutputFbo(fbo);

        // 执行渲染
        node->process(context);
    }

    // 最终输出可以在这里取出：
    // m_outputNode->getOutputFbo() ...

    // 清理：将所有的 FBO 放回对象池
    for (auto fbo : activeFbos) {
        // 实际调用：context.renderer->releaseFBO(fbo);
        delete fbo; // 伪实现直接 delete
    }

    std::cout << "=== [RenderGraph] Frame Rendering Completed ===\n" << std::endl;
}

} // namespace core
} // namespace video_sdk
