#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <atomic>
#include "RenderNode.h"
#include "rhi/interface/IRenderer.h"
#include "core/utils/ThreadPool.h"

namespace video_sdk {
namespace core {

/**
 * @brief 高性能多线程有向无环图 (DAG) 渲染管线引擎。
 * 基于 Data-flow 模型，使用 ThreadPool 并发执行拓扑层级相同的节点。
 */
class RenderGraph {
public:
    // 初始化时注入一个全局共享的线程池，避免频繁创建线程
    explicit RenderGraph(std::shared_ptr<ThreadPool> threadPool);
    ~RenderGraph();

    void addNode(std::shared_ptr<RenderNode> node);
    void setOutputNode(std::shared_ptr<RenderNode> node);

    // 编译图：建立静态的依赖计数和子节点邻接表
    bool compile();

    // 并发渲染一帧，阻塞直到整个图渲染完毕
    void render(RenderContext& context);

private:
    std::vector<std::shared_ptr<RenderNode>> m_nodes;
    std::shared_ptr<RenderNode> m_outputNode;

    std::shared_ptr<ThreadPool> m_threadPool;

    // 编译产物：每个节点的初始入度 (前置依赖数量)
    std::unordered_map<std::shared_ptr<RenderNode>, int> m_initialInDegree;

    // 编译产物：邻接表 (当前节点执行完毕后，需要通知哪些子节点)
    std::unordered_map<std::shared_ptr<RenderNode>, std::vector<std::shared_ptr<RenderNode>>> m_adjList;

    bool buildDependencies();
};

} // namespace core
} // namespace video_sdk
