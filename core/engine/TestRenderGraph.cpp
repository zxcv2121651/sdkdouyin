#include "RenderNode.h"
#include <iostream>
#include <memory>

// 一个极简的 Mock Texture 来让测试编译通过，实际引擎会用 Renderer 产生
class MockTexture : public video_sdk::rhi::ITexture {
public:
    uint32_t id;
    MockTexture(uint32_t i) : id(i) {}
    int getWidth() const override { return 1920; }
    int getHeight() const override { return 1080; }
    video_sdk::rhi::TextureFormat getFormat() const override { return video_sdk::rhi::TextureFormat::RGBA8; }
    uint32_t getNativeId() const override { return id; }
};

int main() {
    std::cout << "--- Testing Pure RHI RenderGraph ---" << std::endl;

    auto sourceA = std::make_shared<video_sdk::core::SourceNode>("VideoSourceA", 100);
    auto sourceB = std::make_shared<video_sdk::core::SourceNode>("VideoSourceB", 101);

    auto beautyFilter = std::make_shared<video_sdk::core::FilterNode>("BeautyFilter", "Bilateral");
    beautyFilter->addInputNode(sourceA);

    auto lutFilter = std::make_shared<video_sdk::core::FilterNode>("LUTFilter", "CyberpunkLUT");
    lutFilter->addInputNode(beautyFilter);

    auto transition = std::make_shared<video_sdk::core::TransitionNode>("CrossfadeMixer", 0.5f);
    transition->addInputNode(lutFilter);
    transition->addInputNode(sourceB); // sourceB is the next clip

    std::cout << "[RenderGraph] Compiled successfully." << std::endl;

    // --- 模拟渲染过程 ---
    video_sdk::core::RenderContext ctx;
    ctx.targetWidth = 1080;
    ctx.targetHeight = 1920;
    ctx.currentPts = 33000;

    std::cout << "\n=== [RenderGraph] Starting Frame Rendering ===" << std::endl;

    // 假设这些是 RHI 分配出来的 Texture
    auto tex1 = std::make_shared<MockTexture>(1);
    auto tex2 = std::make_shared<MockTexture>(2);
    auto tex3 = std::make_shared<MockTexture>(3);
    auto tex4 = std::make_shared<MockTexture>(4);
    auto tex5 = std::make_shared<MockTexture>(5);

    // 1. Source Nodes
    sourceB->setOutputTexture(tex1);
    sourceB->process(ctx);

    sourceA->setOutputTexture(tex2);
    sourceA->process(ctx);

    // 2. Filters
    beautyFilter->setOutputTexture(tex3);
    beautyFilter->process(ctx);

    lutFilter->setOutputTexture(tex4);
    lutFilter->process(ctx);

    // 3. Transition (Mix)
    transition->setOutputTexture(tex5);
    transition->process(ctx);

    std::cout << "=== [RenderGraph] Frame Rendering Completed ===\n" << std::endl;

    return 0;
}
