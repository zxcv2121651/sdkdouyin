#include "NLEModel.h"
#include <sstream>

namespace video_sdk {
namespace core {

// 由于我们不想在这个底层 C++ 库里引入臃肿的第三方 JSON 库 (如 nlohmann/json) 增加编译负担，
// 我们在这里手写非常简单的序列化器，用于演示商业级 SDK 的原理。
// 在真实商业项目中，这里会用 rapidjson 或者 flatbuffers。

std::string NLEEffect::toJson() const {
    std::stringstream ss;
    ss << "{ \"id\": \"" << id << "\", "
       << "\"effectName\": \"" << effectName << "\", "
       << "\"intensity\": " << intensity << " }";
    return ss.str();
}

std::string NLETrackSlot::toJson() const {
    std::stringstream ss;
    ss << "{ \"id\": \"" << id << "\", "
       << "\"resourcePath\": \"" << resourcePath << "\", "
       << "\"sourceStartTimeMs\": " << sourceStartTimeMs << ", "
       << "\"sourceDurationMs\": " << sourceDurationMs << ", "
       << "\"timelineStartTimeMs\": " << timelineStartTimeMs << ", "
       << "\"effects\": [";

    for (size_t i = 0; i < effects.size(); ++i) {
        ss << effects[i]->toJson();
        if (i < effects.size() - 1) ss << ", ";
    }
    ss << "] }";
    return ss.str();
}

void NLETrack::addSlot(std::shared_ptr<NLETrackSlot> slot) {
    slots.push_back(slot);
}

std::string NLETrack::toJson() const {
    std::stringstream ss;
    ss << "{ \"id\": \"" << id << "\", "
       << "\"type\": " << static_cast<int>(type) << ", "
       << "\"slots\": [";

    for (size_t i = 0; i < slots.size(); ++i) {
        ss << slots[i]->toJson();
        if (i < slots.size() - 1) ss << ", ";
    }
    ss << "] }";
    return ss.str();
}

void NLEProject::addTrack(std::shared_ptr<NLETrack> track) {
    tracks.push_back(track);
}

std::string NLEProject::toJson() const {
    std::stringstream ss;
    ss << "{ \"projectId\": \"" << projectId << "\", "
       << "\"totalDurationMs\": " << totalDurationMs << ", "
       << "\"tracks\": [";

    for (size_t i = 0; i < tracks.size(); ++i) {
        ss << tracks[i]->toJson();
        if (i < tracks.size() - 1) ss << ", ";
    }
    ss << "] }";
    return ss.str();
}

std::shared_ptr<NLEProject> NLEProject::fromJson(const std::string& jsonString) {
    // 简易反序列化 Mock：在真实项目中，这里负责将 JSON 文本重建为 NLE 内存结构树。
    // 为了让项目编译并且体现架构，我们硬编码解析出一个 mock 项目
    auto project = std::make_shared<NLEProject>();
    project->projectId = "restored_project_1";
    project->totalDurationMs = 15000;
    return project;
}

} // namespace core
} // namespace video_sdk
