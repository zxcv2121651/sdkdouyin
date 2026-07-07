#include "RenderGraph.h"
#include <iostream>

using namespace video_sdk::core;

int main() {
    std::cout << "--- Testing RenderGraph ---" << std::endl;

    auto threadPool = std::make_shared<ThreadPool>(4);
    RenderGraph graph(threadPool);

    // 创建节点
    auto sourceA = std::make_shared<SourceNode>("VideoSourceA", 100);
    auto sourceB = std::make_shared<SourceNode>("VideoSourceB", 101);

    auto beautyFilter = std::make_shared<FilterNode>("BeautyFilter", "Bilateral");
    auto lutFilter = std::make_shared<FilterNode>("LUTFilter", "CyberpunkLUT");

    auto transition = std::make_shared<TransitionNode>("CrossfadeMixer", 0.5f);

    // 构建有向无环图 (DAG) 拓扑依赖：
    // SourceA -> BeautyFilter -> LUTFilter -\
    //                                        -> Transition -> (Output)
    // SourceB -----------------------------/

    beautyFilter->addInputNode(sourceA);
    lutFilter->addInputNode(beautyFilter);

    transition->addInputNode(lutFilter); // input 0: 处理后的 A
    transition->addInputNode(sourceB);   // input 1: 原始的 B

    // 添加到图中
    graph.addNode(sourceA);
    graph.addNode(sourceB);
    graph.addNode(beautyFilter);
    graph.addNode(lutFilter);
    graph.addNode(transition);
    graph.setOutputNode(transition);

    // 编译（计算执行顺序）
    if (graph.compile()) {
        // 渲染一帧
        RenderContext context;
        context.targetWidth = 1080;
        context.targetHeight = 1920;
        context.currentPts = 33000;

        graph.render(context);
    }

    return 0;
}
