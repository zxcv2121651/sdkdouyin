#pragma once
#include <string>
#include <memory>
#include <vector>
#include "core/timeline/Timeline.h"

namespace video_sdk {
namespace modules {

/**
 * @brief 面向业务的视频编辑器 API (Facade 门面模式)。
 *
 * 核心架构思想：
 * 业务层（比如 Kotlin 或 Swift 的 UI 代码）绝对不应该直接操作和拼装
 * `RenderNode` 或 `Clip` 这样复杂的底层结构。
 *
 * 这个类提供了一套对人类极度友好的命令式（Command-like）API。
 * 当你在界面上点击“切割视频”时，实际上是调用了 `splitClip`，
 * 引擎内部会自动把一个 Clip 劈成两半，并计算好它们新的 `TimelineIn` 和 `TimelineOut`。
 */
class VideoEditor {
public:
    VideoEditor();
    ~VideoEditor();

    // --- 基础时间线操作 (Timeline CRUD) ---

    /**
     * @brief 导入一段视频或照片，插入到时间轴的指定位置。
     * @param filePath 本地文件路径
     * @param startTimeMs 放在全局时间轴的哪一毫秒开始
     * @return 返回引擎为这个素材生成的唯一 ID，后续所有的修剪、加滤镜都靠这个 ID。
     */
    std::string importVideo(const std::string& filePath, int64_t startTimeMs);

    // 删除指定的媒体片段
    bool removeClip(const std::string& clipId);

    /**
     * @brief 切割：将一个视频在给定时间点一刀劈成两半
     * 引擎内部会克隆源 Clip，并修改前一段的 Out 点和后一段的 In 点。
     * @return 返回被切出来的新半段的 ID
     */
    std::string splitClip(const std::string& clipId, int64_t splitTimeMs);

    // 修剪片段：修改视频在时间线上出现的总长，掐头去尾
    bool trimClip(const std::string& clipId, int64_t trimInMs, int64_t trimOutMs);

    // --- 高阶属性修改 (Properties) ---

    // 改变片段速度 (变慢/变快)，底层会影响 mapTimelineToSourceTime 的斜率计算
    bool setClipSpeed(const std::string& clipId, float speedRatio);

    // 改变片段音量
    bool setClipVolume(const std::string& clipId, float volume);

    /**
     * @brief 在两个片段之间插入转场动画 (如交叉溶解 Crossfade)
     * 引擎底层会自动在 RenderGraph 中插入一个 TransitionNode，
     * 并在时间线上将两段视频拉出重叠区域进行像素混合。
     */
    void addTransition(const std::string& clipId1, const std::string& clipId2, const std::string& transitionType, int64_t durationMs);

    // --- 内部获取 ---
    // 供底层的 VideoPlayer 和 TimelineExporter 挂载并读取时间线数据
    std::shared_ptr<core::Timeline> getTimeline() const { return m_timeline; }

private:
    std::shared_ptr<core::Timeline> m_timeline;
    int m_clipCounter = 0;

    std::string generateClipId();
};

} // namespace modules
} // namespace video_sdk
