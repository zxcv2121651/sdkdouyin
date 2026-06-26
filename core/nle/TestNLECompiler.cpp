#include "NLEModel.h"
#include "NLECompiler.h"
#include <iostream>

using namespace video_sdk::core;

int main() {
    std::cout << "--- Testing NLE Model Serialization & Compiler ---" << std::endl;

    auto project = std::make_shared<NLEProject>();
    project->projectId = "test_project_888";

    auto videoTrack = std::make_shared<NLETrack>();
    videoTrack->id = "track_v1";
    videoTrack->type = NLETrackType::VIDEO;

    auto clip1 = std::make_shared<NLETrackSlot>();
    clip1->id = "clip_01";
    clip1->resourcePath = "/sdcard/1.mp4";

    // Add Beauty Filter to test the Compute Shader path
    auto effect1 = std::make_shared<NLEEffect>();
    effect1->id = "eff_1";
    effect1->effectName = "BeautyFilter";
    effect1->intensity = 0.8f;
    clip1->effects.push_back(effect1);

    auto clip2 = std::make_shared<NLETrackSlot>();
    clip2->id = "clip_02";
    clip2->resourcePath = "/sdcard/2.mp4";

    videoTrack->addSlot(clip1);
    videoTrack->addSlot(clip2);
    project->addTrack(videoTrack);

    std::cout << "\n[Serialization] Generating JSON Draft:" << std::endl;
    std::string draftJson = project->toJson();
    std::cout << draftJson << std::endl;

    std::cout << "\n[Compiler] Translating Draft to RenderGraph:" << std::endl;
    NLECompiler compiler;
    auto graph = compiler.compileProject(project);

    if (graph) {
        std::cout << "\n[RenderGraph] Executing generated pipeline:" << std::endl;
        RenderContext ctx;
        ctx.targetWidth = 720;
        ctx.targetHeight = 1280;
        graph->render(ctx);
    }

    std::cout << "--- Testing Completed ---" << std::endl;
    return 0;
}
