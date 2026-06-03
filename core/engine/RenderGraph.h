#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include "RenderNode.h"
#include "rhi/interface/IRenderer.h"

namespace video_sdk {
namespace core {

/**
 * @brief 有向无环图渲染管线引擎。
 * 负责解析节点的拓扑依赖关系，并自动分配/回收 FBO，确保执行顺序正确。
 */
class RenderGraph {
public:
    RenderGraph();
    ~RenderGraph();

    // 添加节点到图中
    void addNode(std::shared_ptr<RenderNode> node);

    // 设置该图最终输出的目标节点
    void setOutputNode(std::shared_ptr<RenderNode> node);

    // 编译图：进行拓扑排序，生成执行序列
    bool compile();

    // 渲染一帧
    void render(RenderContext& context);

private:
    std::vector<std::shared_ptr<RenderNode>> m_nodes;
    std::shared_ptr<RenderNode> m_outputNode;

    // 拓扑排序后的执行序列
    std::vector<std::shared_ptr<RenderNode>> m_executionSequence;

    // Kahn's 算法进行拓扑排序
    bool topologicalSort();
};

} // namespace core
} // namespace video_sdk
