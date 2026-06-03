#pragma once
#include <string>
#include <memory>
#include <vector>
#include "core/timeline/Timeline.h"

namespace video_sdk {
namespace modules {

/**
 * @brief 面向业务的视频编辑器 API。
 * 封装底层的 Timeline，提供易用的增删改查及特效叠加操作。
 */
class VideoEditor {
public:
    VideoEditor();
    ~VideoEditor();

    // --- 基础增删改查 (CRUD) ---

    // 导入视频并返回其生成的唯一 ID
    std::string importVideo(const std::string& filePath, int64_t startTimeMs);

    // 删除指定的片段
    bool removeClip(const std::string& clipId);

    // 切割片段：将一个视频在给定时间点分为两段 (返回后半段的 ID)
    std::string splitClip(const std::string& clipId, int64_t splitTimeMs);

    // 修剪片段：修改视频头尾
    bool trimClip(const std::string& clipId, int64_t trimInMs, int64_t trimOutMs);

    // --- 高阶属性修改 ---

    // 改变片段速度 (变慢/变快)
    bool setClipSpeed(const std::string& clipId, float speedRatio);

    // 改变片段音量
    bool setClipVolume(const std::string& clipId, float volume);

    // 添加转场动画
    void addTransition(const std::string& clipId1, const std::string& clipId2, const std::string& transitionType, int64_t durationMs);

    // --- 内部获取 ---
    std::shared_ptr<core::Timeline> getTimeline() const { return m_timeline; }

private:
    std::shared_ptr<core::Timeline> m_timeline;
    int m_clipCounter = 0;

    std::string generateClipId();
};

} // namespace modules
} // namespace video_sdk
