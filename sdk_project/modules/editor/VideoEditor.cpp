#include "core/timeline/Clip.h"
#include "VideoEditor.h"
#include <iostream>

namespace video_sdk {
namespace modules {

VideoEditor::VideoEditor() {
    m_timeline = std::make_shared<core::Timeline>();
}

VideoEditor::~VideoEditor() {}

std::string VideoEditor::generateClipId() {
    m_clipCounter++;
    return "clip_" + std::to_string(m_clipCounter);
}

std::string VideoEditor::importVideo(const std::string& filePath, int64_t startTimeMs) {
    std::string newId = generateClipId();
    auto clip = std::make_shared<core::Clip>(newId, filePath);

    // 假设默认素材时长为 5000ms
    int64_t defaultDuration = 5000;
    clip->setSourceRange(0, defaultDuration);
    clip->setTimelineRange(startTimeMs, startTimeMs + defaultDuration);

    m_timeline->addClip("main_video_track", clip);
    std::cout << "[VideoEditor] Imported clip: " << newId << " at " << startTimeMs << "ms" << std::endl;
    return newId;
}

bool VideoEditor::removeClip(const std::string& clipId) {
    if (!m_timeline->getClip(clipId)) return false;
    m_timeline->removeClip(clipId);
    std::cout << "[VideoEditor] Removed clip: " << clipId << std::endl;
    return true;
}

std::string VideoEditor::splitClip(const std::string& clipId, int64_t splitTimeMs) {
    auto clip = m_timeline->getClip(clipId);
    if (!clip || !clip->containsTimelineTime(splitTimeMs)) return "";

    // 1. 获取原来的出入点
    int64_t originalTimelineOut = clip->getTimelineOut();
    int64_t sourceSplitPoint = clip->mapTimelineToSourceTime(splitTimeMs);
    int64_t originalSourceOut = clip->getSourceOut();

    // 2. 修改前半段（原 Clip）的出点
    clip->setTimelineRange(clip->getTimelineIn(), splitTimeMs);
    clip->setSourceRange(clip->getSourceIn(), sourceSplitPoint);
    // 更新底层排序
    m_timeline->updateClipRange(clipId, clip->getTimelineIn(), splitTimeMs);

    // 3. 创建后半段（新 Clip）
    std::string newId = generateClipId();
    auto newClip = std::make_shared<core::Clip>(newId, clip->getSourcePath());
    newClip->setSourceRange(sourceSplitPoint, originalSourceOut);
    newClip->setTimelineRange(splitTimeMs, originalTimelineOut);

    m_timeline->addClip("main_video_track", newClip);
    std::cout << "[VideoEditor] Split clip " << clipId << " into " << clipId << " and " << newId << std::endl;
    return newId;
}

bool VideoEditor::trimClip(const std::string& clipId, int64_t trimInMs, int64_t trimOutMs) {
    auto clip = m_timeline->getClip(clipId);
    if (!clip) return false;
    m_timeline->updateClipRange(clipId, trimInMs, trimOutMs);
    std::cout << "[VideoEditor] Trimmed clip " << clipId << std::endl;
    return true;
}

bool VideoEditor::setClipSpeed(const std::string& clipId, float speedRatio) {
    auto clip = m_timeline->getClip(clipId);
    if (!clip) return false;
    // ... 将速度属性存入 Clip 实例，渲染引擎计算 PTS 偏移时会使用
    return true;
}

bool VideoEditor::setClipVolume(const std::string& clipId, float volume) {
    auto clip = m_timeline->getClip(clipId);
    if (!clip) return false;
    // ... 将音量属性存入，AudioMixer 处理时会将其作为 Gain 值乘入
    return true;
}

void VideoEditor::addTransition(const std::string& clipId1, const std::string& clipId2, const std::string& transitionType, int64_t durationMs) {
    // 工业级做法是往特定的 TransitionTrack 里面插入一个 Transition 对象，
    // 该对象跨越 clipId1 的尾部和 clipId2 的头部，并在 RenderGraph 解析时生成相应的双输入节点。
}

} // namespace modules
} // namespace video_sdk
