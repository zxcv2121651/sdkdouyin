#include "VideoEditor.h"

namespace video_sdk {
namespace modules {

VideoEditor::VideoEditor() {
    m_timeline = std::make_shared<core::Timeline>();
}

VideoEditor::~VideoEditor() {}

void VideoEditor::importVideo(const std::string& filePath, int64_t startTimeMs) {
    // 1. 创建 VideoTrack (如果不存在)
    // 2. 根据 filePath 构建一个 VideoClip
    // 3. m_timeline->addClip("video_track_0", clip);
}

void VideoEditor::trimClip(const std::string& clipId, int64_t trimInMs, int64_t trimOutMs) {
    // 查找到指定的 Clip，修改其时间入点和出点，并重排序 Timeline 的缓存数组
}

void VideoEditor::addTransition(const std::string& clipId1, const std::string& clipId2, const std::string& transitionType, int64_t durationMs) {
    // 在 Timeline 的转场管理器中注入一个 Transition 对象
    // 这个对象会在渲染引擎合成时触发对应的转场 Shader
}

} // namespace modules
} // namespace video_sdk
