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

/**
 * @brief 执行拓扑排序 (Kahn's Algorithm)，将有向无环图 (DAG) 展平为一维执行队列。
 * 这是现代渲染引擎的核心算法，确保节点 B 在处理节点 A 的输出时，节点 A 一定已经渲染完毕。
 *
 * @return 排序成功返回 true。如果图中存在环状依赖 (Cyclic dependency) 则返回 false。
 */
bool RenderGraph::topologicalSort() {
    m_executionSequence.clear();

    // 记录每个节点的入度 (In-Degree) 和邻接表 (Adjacency List)
    std::unordered_map<std::shared_ptr<RenderNode>, int> inDegree;
    std::unordered_map<std::shared_ptr<RenderNode>, std::vector<std::shared_ptr<RenderNode>>> adjList;

    for (auto& node : m_nodes) {
        inDegree[node] = 0;
    }

    // 构建图结构: 数据流向是 input -> current
    for (auto& node : m_nodes) {
        for (auto& input : node->getInputNodes()) {
            adjList[input].push_back(node);
            inDegree[node]++;
        }
    }

    // 寻找所有入度为 0 的节点 (即没有前置依赖的起点, 通常是 VideoSourceNode 或 ImageSourceNode)
    std::queue<std::shared_ptr<RenderNode>> zeroInDegreeQueue;
    for (auto& pair : inDegree) {
        if (pair.second == 0) {
            zeroInDegreeQueue.push(pair.first);
        }
    }

    // 剥洋葱：每次取出一个零入度节点加入执行队列，并消除它的出边
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

    // 防御性编程：检查是否有无法消除入度的节点（意味着图中存在死循环环路）
    if (m_executionSequence.size() != m_nodes.size()) {
        std::cerr << "[RenderGraph] ERROR: Cyclic dependency detected!" << std::endl;
        return false;
    }

    std::cout << "[RenderGraph] Compiled successfully. Sequence: ";
    for (auto& n : m_executionSequence) std::cout << n->getName() << " -> ";
    std::cout << "END" << std::endl;

    return true;
}

/**
 * @brief 执行单帧的完整渲染管线流水线。
 * @param context 渲染上下文，包含了全局共享资源 (如 Renderer 实例)
 */
void RenderGraph::render(RenderContext& context) {
    if (m_executionSequence.empty()) return;

    std::cout << "\n=== [RenderGraph] Starting Frame Rendering ===" << std::endl;

    // 用于记录本帧申请的 FBO(Frame Buffer Object)，渲染结束后统一自动回收
    std::vector<rhi::FrameBufferObject*> activeFbos;

    for (auto& node : m_executionSequence) {
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
    }

    std::cout << "=== [RenderGraph] Frame Rendering Completed ===\n" << std::endl;
}

} // namespace core
} // namespace video_sdk
