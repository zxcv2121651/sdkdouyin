#include "RenderNode.h"
#include "RenderGraph.h"
#include "rhi/DefaultFBOPool.h"
#include <iostream>
#include <memory>

class MockRenderer : public video_sdk::rhi::IRenderer {
public:
    uint32_t fboCounter = 1;
    void initialize() override {}
    void destroy() override {}
    const video_sdk::rhi::RendererCapabilities& getCapabilities() const override { static video_sdk::rhi::RendererCapabilities c; return c; }

    video_sdk::rhi::FrameBufferObject* acquireFBO(int width, int height) override {
        std::cout << "[MockRenderer] Actually allocating FBO: " << fboCounter << std::endl;
        return new video_sdk::rhi::FrameBufferObject{fboCounter++, fboCounter * 10, width, height};
    }

    void releaseFBO(video_sdk::rhi::FrameBufferObject* fbo) override {
        std::cout << "[MockRenderer] Releasing FBO: " << fbo->fboId << std::endl;
        delete fbo;
    }

    uint32_t compileShader(const std::string& v, const std::string& f) override { return 1; }
    uint32_t compileComputeShader(const std::string& c) override { return 1; }
    void dispatchCompute(uint32_t p, int x, int y, int z) override {}
};

int main() {
    std::cout << "--- Testing FBO Pool RenderGraph ---" << std::endl;

    auto sourceA = std::make_shared<video_sdk::core::SourceNode>("VideoSourceA", 100);
    auto sourceB = std::make_shared<video_sdk::core::SourceNode>("VideoSourceB", 101);

    auto beautyFilter = std::make_shared<video_sdk::core::FilterNode>("BeautyFilter", "Bilateral");
    beautyFilter->addInputNode(sourceA);

    auto transition = std::make_shared<video_sdk::core::TransitionNode>("CrossfadeMixer", 0.5f);
    transition->addInputNode(beautyFilter);
    transition->addInputNode(sourceB);

    video_sdk::core::RenderGraph graph;
    graph.addNode(sourceA);
    graph.addNode(sourceB);
    graph.addNode(beautyFilter);
    graph.setOutputNode(transition);

    graph.compile();

    auto mockRenderer = std::make_shared<MockRenderer>();
    auto fboPool = std::make_shared<video_sdk::rhi::DefaultFBOPool>(mockRenderer);

    video_sdk::core::RenderContext ctx;
    ctx.renderer = mockRenderer;
    ctx.fboPool = fboPool;
    ctx.targetWidth = 1080;
    ctx.targetHeight = 1920;
    ctx.currentPts = 33000;

    std::cout << "\n=== [RenderGraph] Frame 1 (Cold Start) ===" << std::endl;
    graph.render(ctx);

    ctx.currentPts = 66000;
    std::cout << "\n=== [RenderGraph] Frame 2 (Should reuse pooled FBOs) ===" << std::endl;
    graph.render(ctx);

    ctx.currentPts = 99000;
    std::cout << "\n=== [RenderGraph] Frame 3 (Should reuse pooled FBOs) ===" << std::endl;
    graph.render(ctx);

    std::cout << "\n=== [RenderGraph] Testing Completed ===\n" << std::endl;

    return 0;
}
