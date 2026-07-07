#include "RenderGraph.h"
#include <queue>
#include <iostream>

namespace video_sdk {
namespace core {

RenderGraph::RenderGraph(std::shared_ptr<ThreadPool> threadPool)
    : m_threadPool(std::move(threadPool)) {}

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
    if (!m_threadPool) {
        std::cerr << "[RenderGraph] ERROR: ThreadPool is null!" << std::endl;
        return false;
    }
    return buildDependencies();
}

/**
 * @brief 构建静态依赖图（邻接表和初始入度），并校验是否有环路。
 */
bool RenderGraph::buildDependencies() {
    m_initialInDegree.clear();
    m_adjList.clear();

    // 初始化
    for (auto& node : m_nodes) {
        m_initialInDegree[node] = 0;
    }

    // 构建数据流向图: input -> current
    for (auto& node : m_nodes) {
        for (auto& input : node->getInputNodes()) {
            m_adjList[input].push_back(node);
            m_initialInDegree[node]++;
        }
    }

    // 利用 Kahn 算法校验一遍是否有环路
    std::unordered_map<std::shared_ptr<RenderNode>, int> tempInDegree = m_initialInDegree;
    std::queue<std::shared_ptr<RenderNode>> q;
    for (auto& pair : tempInDegree) {
        if (pair.second == 0) q.push(pair.first);
    }

    int processedCount = 0;
    while (!q.empty()) {
        auto current = q.front();
        q.pop();
        processedCount++;
        for (auto& neighbor : m_adjList[current]) {
            if (--tempInDegree[neighbor] == 0) {
                q.push(neighbor);
            }
        }
    }

    if (processedCount != m_nodes.size()) {
        std::cerr << "[RenderGraph] ERROR: Cyclic dependency detected!" << std::endl;
        return false;
    }

    std::cout << "[RenderGraph] Compiled dependencies successfully for concurrent execution." << std::endl;
    return true;
}

/**
 * @brief 执行单帧的并发渲染管线流水线。
 */
void RenderGraph::render(RenderContext& context) {
    if (m_nodes.empty()) return;

    std::cout << "\n=== [RenderGraph] Starting Concurrent Frame Rendering ===" << std::endl;

    // 用于记录本帧申请的 FBO，渲染结束后统一自动回收
    std::vector<rhi::FrameBufferObject*> activeFbos;
    std::mutex fboMutex;

    // 动态的运行时期依赖计数器
    std::unordered_map<std::shared_ptr<RenderNode>, std::atomic<int>> currentInDegree;
    for (auto& pair : m_initialInDegree) {
        currentInDegree[pair.first].store(pair.second, std::memory_order_relaxed);
    }

    // 统计已完成的节点数量，用于最终同步
    std::atomic<int> completedNodesCount{0};
    std::condition_variable cv;
    std::mutex cvMutex;

    // 内部递归投递函数
    std::function<void(std::shared_ptr<RenderNode>)> dispatchNode = [&](std::shared_ptr<RenderNode> node) {
        m_threadPool->enqueue([&, node]() {

            // 1. 为该节点分配 FBO
            rhi::FrameBufferObject* fbo = nullptr;
            if (context.fboPool) {
                fbo = context.fboPool->acquireFBO(context.targetWidth, context.targetHeight);
            } else {
                static std::atomic<uint32_t> pseudoFboCounter{1};
                uint32_t id = pseudoFboCounter.fetch_add(1);
                fbo = new rhi::FrameBufferObject{id, id * 10, context.targetWidth, context.targetHeight};
            }

            {
                std::lock_guard<std::mutex> lock(fboMutex);
                activeFbos.push_back(fbo);
            }
            node->setOutputFbo(fbo);

            // 2. 执行真实的 DrawCall (在工作线程中)
            // std::cout << "  -> Thread processing node: " << node->getName() << std::endl;
            node->process(context);

            // 3. 递减子节点的依赖计数 (Data-flow 驱动)
            for (auto& child : m_adjList[node]) {
                if (currentInDegree[child].fetch_sub(1, std::memory_order_acq_rel) == 1) {
                    // 当计数器从 1 变为 0 时，说明其所有前置依赖已完成，可以立即入队执行
                    dispatchNode(child);
                }
            }

            // 4. 检查是否整个图都执行完毕
            if (completedNodesCount.fetch_add(1, std::memory_order_acq_rel) == m_nodes.size() - 1) {
                // 唤醒主线程
                std::lock_guard<std::mutex> lock(cvMutex);
                cv.notify_one();
            }
        });
    };

    // 触发图的执行：将所有无前置依赖的起点节点放入线程池
    for (auto& pair : m_initialInDegree) {
        if (pair.second == 0) {
            dispatchNode(pair.first);
        }
    }

    // 阻塞主线程，等待所有节点在线程池中执行完毕
    {
        std::unique_lock<std::mutex> lock(cvMutex);
        cv.wait(lock, [&]() { return completedNodesCount.load(std::memory_order_acquire) == m_nodes.size(); });
    }

    // 渲染结束，清理阶段：将所有 FBO 放回对象池以供下一帧复用
    for (auto fbo : activeFbos) {
        if (context.fboPool) {
            context.fboPool->releaseFBO(fbo);
        } else {
            delete fbo;
        }
    }

    std::cout << "=== [RenderGraph] Frame Rendering Completed ===\n" << std::endl;
}

} // namespace core
} // namespace video_sdk
