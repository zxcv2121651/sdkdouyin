#pragma once
#include <string>
#include <cstdint>

namespace video_sdk {
namespace core {

/**
 * @brief Clip 表示非线性编辑（NLE）时间线上的一段基础素材实体（如视频片段、音频片段、贴纸）。
 *
 * 核心概念：
 * 在 NLE 系统中，Clip 维护了两套时间坐标系：
 * 1. Source Time (源时间)：这段素材在其原始文件中的时间区间。
 * 2. Timeline Time (时间线时间)：这段素材被放置在全局剪辑工程时间线上的具体位置。
 * 这两套时间的映射(Mapping)是实现视频剪切(Trim)、变速(Speed)等功能的基础。
 */
class Clip {
public:
    /**
     * @param id 唯一标识符，通常由上层（如 UUID）生成并传入
     * @param sourcePath 素材在设备上的物理路径或网络 URL
     */
    Clip(const std::string& id, const std::string& sourcePath);
    ~Clip() = default;

    // --- 属性 Getter / Setter ---
    std::string getId() const { return m_id; }
    std::string getSourcePath() const { return m_sourcePath; }

    /**
     * @brief 设置该片段在主时间线上的出现区间 (毫秒)
     * 例如，将其放置在主视频的第 5 秒到第 10 秒处。
     */
    void setTimelineRange(int64_t inMs, int64_t outMs);

    /**
     * @brief 设置对源媒体文件的裁剪区间 (毫秒)
     * 例如，只需要原视频文件从第 2 秒到第 7 秒的内容。
     */
    void setSourceRange(int64_t inMs, int64_t outMs);

    int64_t getTimelineIn() const { return m_timelineIn; }
    int64_t getTimelineOut() const { return m_timelineOut; }
    int64_t getSourceIn() const { return m_sourceIn; }
    int64_t getSourceOut() const { return m_sourceOut; }

    // --- 核心时间映射逻辑 ---

    /**
     * @brief 判断给定的时间线绝对时间是否落在该 Clip 的播放区间内。
     * 用于播放器引擎在某一帧决定是否需要拉起该 Clip 的解码器进行解码。
     * @param timelineTimeMs 全局播放时间
     * @return 如果落在区间内返回 true，否则返回 false
     */
    bool containsTimelineTime(int64_t timelineTimeMs) const;

    /**
     * @brief 核心映射算法：将全局的时间线时间转换回源文件的时间点。
     * 播放器知道现在播放到了第 10 秒，需要通过此函数计算出应该向 FFmpeg 或 MediaCodec
     * 请求当前视频文件的第几秒的画面。
     * @param timelineTimeMs 全局播放时间
     * @return 源文件的绝对时间点（毫秒），如果变速，这里会包含斜率计算。
     */
    int64_t mapTimelineToSourceTime(int64_t timelineTimeMs) const;

private:
    std::string m_id;
    std::string m_sourcePath;

    // 在全局时间线上的绝对坐标 (入点和出点)
    int64_t m_timelineIn = 0;
    int64_t m_timelineOut = 0;

    // 实际截取源素材文件的内部坐标 (入点和出点)
    int64_t m_sourceIn = 0;
    int64_t m_sourceOut = 0;
};

} // namespace core
} // namespace video_sdk
