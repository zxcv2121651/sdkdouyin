#pragma once
#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace video_sdk {
namespace core {

enum class NLETrackType {
    VIDEO,
    AUDIO,
    EFFECT
};

// ---------------------------------------------------------
// NLEEffect (特效数据模型)
// ---------------------------------------------------------
class NLEEffect {
public:
    std::string id;
    std::string effectName;
    float intensity = 1.0f;

    std::string toJson() const;
    void fromJson(const std::string& jsonString); // 简化实现，实际应用中会使用 json 库
};

// ---------------------------------------------------------
// NLETrackSlot (轨道上的一个片段，例如一段视频，或一个转场)
// ---------------------------------------------------------
class NLETrackSlot {
public:
    std::string id;
    std::string resourcePath; // 本地文件路径

    // 素材原本的时间范围 (比如剪裁后的素材是从第2秒到第5秒)
    int64_t sourceStartTimeMs = 0;
    int64_t sourceDurationMs = 0;

    // 该片段在主时间轴上的位置
    int64_t timelineStartTimeMs = 0;

    // 挂载在这个片段上的特效
    std::vector<std::shared_ptr<NLEEffect>> effects;

    std::string toJson() const;
};

// ---------------------------------------------------------
// NLETrack (轨道)
// ---------------------------------------------------------
class NLETrack {
public:
    std::string id;
    NLETrackType type;
    std::vector<std::shared_ptr<NLETrackSlot>> slots;

    void addSlot(std::shared_ptr<NLETrackSlot> slot);
    std::string toJson() const;
};

// ---------------------------------------------------------
// NLEProject (完整项目草稿)
// ---------------------------------------------------------
class NLEProject {
public:
    std::string projectId;
    int64_t totalDurationMs = 0;
    std::vector<std::shared_ptr<NLETrack>> tracks;

    void addTrack(std::shared_ptr<NLETrack> track);
    std::string toJson() const;

    // 从 JSON 文本重建整个项目
    static std::shared_ptr<NLEProject> fromJson(const std::string& jsonString);
};

} // namespace core
} // namespace video_sdk
