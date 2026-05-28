#pragma once
#include <string>
#include <cstdint>

namespace video_sdk {
namespace core {

/**
 * @brief Clip 表示时间线上的一段基础素材（如视频、音频、贴纸）。
 * 包含了源媒体的裁剪区间以及映射到主时间线上的区间。
 */
class Clip {
public:
    Clip(const std::string& id, const std::string& sourcePath);
    ~Clip() = default;

    // --- 属性 Getter / Setter ---
    std::string getId() const { return m_id; }
    std::string getSourcePath() const { return m_sourcePath; }

    // 设置在主时间轴上的入点和出点 (毫秒)
    void setTimelineRange(int64_t inMs, int64_t outMs);

    // 设置裁剪源文件的入点和出点 (毫秒)
    void setSourceRange(int64_t inMs, int64_t outMs);

    int64_t getTimelineIn() const { return m_timelineIn; }
    int64_t getTimelineOut() const { return m_timelineOut; }
    int64_t getSourceIn() const { return m_sourceIn; }
    int64_t getSourceOut() const { return m_sourceOut; }

    // --- 核心时间映射逻辑 ---
    // 判断给定的时间线时间点是否落在该片段内
    bool containsTimelineTime(int64_t timelineTimeMs) const;

    // 将时间线时间点转换为源文件的时间点（用于向 Decoder 请求对应帧）
    int64_t mapTimelineToSourceTime(int64_t timelineTimeMs) const;

private:
    std::string m_id;
    std::string m_sourcePath;

    // 在全局时间线上的坐标
    int64_t m_timelineIn = 0;
    int64_t m_timelineOut = 0;

    // 在源素材文件中的坐标
    int64_t m_sourceIn = 0;
    int64_t m_sourceOut = 0;
};

} // namespace core
} // namespace video_sdk
