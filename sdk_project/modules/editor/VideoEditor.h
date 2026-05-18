#pragma once
#include <string>
#include <memory>
#include "core/timeline/Timeline.h"

namespace video_sdk {
namespace modules {

/**
 * @brief 非线性编辑（NLE）的上层编辑器接口。
 * 面向应用层，对下封装了核心引擎的 Timeline 数据结构。
 */
class VideoEditor {
public:
    VideoEditor();
    ~VideoEditor();

    // 导入视频资源并创建一个新的 Track 和 Clip
    void importVideo(const std::string& filePath, int64_t startTimeMs);

    // 对特定 Clip 进行裁剪
    void trimClip(const std::string& clipId, int64_t trimInMs, int64_t trimOutMs);

    // 添加转场动画（作用于两个相邻的 Clip 之间）
    void addTransition(const std::string& clipId1, const std::string& clipId2, const std::string& transitionType, int64_t durationMs);

    // 获取底层的 Timeline 实例引用（以便传给 Player 进行渲染）
    std::shared_ptr<core::Timeline> getTimeline() const { return m_timeline; }

private:
    std::shared_ptr<core::Timeline> m_timeline;
};

} // namespace modules
} // namespace video_sdk
