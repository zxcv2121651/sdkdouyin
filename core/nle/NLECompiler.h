#pragma once
#include <memory>
#include "NLEModel.h"
#include "core/engine/RenderGraph.h"

namespace video_sdk {
namespace core {

/**
 * @brief NLE Compiler (核心引擎翻译器)
 * 商业级 SDK 的大脑。
 * 负责读取上层的 Data Model (JSON/NLEProject)，并自动生成与之对应的
 * RenderGraph 和相关的 Node 连线，供底层的 RHI 管线执行。
 */
class NLECompiler {
public:
    NLECompiler();
    ~NLECompiler();

    /**
     * @brief 将 NLEProject 编译为可执行的 RenderGraph
     * @param project 传入的数据模型草稿
     * @return 返回构建完毕并经过拓扑排序的 RenderGraph
     */
    std::shared_ptr<RenderGraph> compileProject(std::shared_ptr<NLEProject> project);

private:
    uint32_t m_mockTextureCounter = 100;
};

} // namespace core
} // namespace video_sdk
