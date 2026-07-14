#include "NLECompiler.h"
#include "core/engine/RenderNode.h"
#include <iostream>

namespace video_sdk {
namespace core {

NLECompiler::NLECompiler() {}
NLECompiler::~NLECompiler() {}

std::shared_ptr<RenderGraph> NLECompiler::compileProject(std::shared_ptr<NLEProject> project) {
    if (!project) return nullptr;

    std::cout << "[NLECompiler] Starting to translate NLEProject (ID: " << project->projectId << ") into RenderGraph..." << std::endl;
    auto threadPool = std::make_shared<ThreadPool>(4);
    auto graph = std::make_shared<RenderGraph>(threadPool);

    std::shared_ptr<RenderNode> previousNode = nullptr;

    for (const auto& track : project->tracks) {
        if (track->type == NLETrackType::VIDEO) {
            for (const auto& slot : track->slots) {
                // 1. 创建视频源节点
                auto sourceNode = std::make_shared<SourceNode>("VideoSource_" + slot->id, m_mockTextureCounter++);
                graph->addNode(sourceNode);

                std::shared_ptr<RenderNode> currentNode = sourceNode;

                // 2. 依次挂载挂在它上面的特效
                for (const auto& effect : slot->effects) {
                    auto filterNode = std::make_shared<FilterNode>("Filter_" + effect->id, effect->effectName);
                    filterNode->addInputNode(currentNode);
                    graph->addNode(filterNode);
                    currentNode = filterNode;
                }

                // 3. (简化处理) 如果有前一个片段，我们在这里默认生成一个转场连接它们
                if (previousNode) {
                    auto transitionNode = std::make_shared<TransitionNode>("AutoMixer", 0.5f);
                    transitionNode->addInputNode(previousNode);
                    transitionNode->addInputNode(currentNode);
                    graph->addNode(transitionNode);
                    currentNode = transitionNode;
                }

                previousNode = currentNode;
            }
        }
    }

    if (previousNode) {
        graph->setOutputNode(previousNode);
    }

    std::cout << "[NLECompiler] Translation completed. Triggering internal compilation..." << std::endl;
    graph->compile();

    return graph;
}

} // namespace core
} // namespace video_sdk
