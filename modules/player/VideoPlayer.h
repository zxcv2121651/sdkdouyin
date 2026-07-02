
#include "core/nle/NLEPreloadManager.h"
#pragma once
#include <string>
#include <cstdint>
#include <memory>
#include "core/utils/RenderThread.h"

namespace video_sdk {
namespace modules {

/**
 * @brief 高性能播放器组件。
 * 包装了核心的 timeline，提供针对上层 UI 的基础控制接口（播放、暂停、Seek 等）。
 * 结合 media 层的 AVSyncClock 保证播放过程中的音视频同步。
 * 所有影响到底层渲染管线的操作都将被抛至统一的 RenderThread 中执行。
 */
class VideoPlayer {
public:
    VideoPlayer();
    ~VideoPlayer();

    // 绑定 Android Surface / iOS Layer 以供渲染
    void setWindow(void* window);

    // 加载资源并准备播放
    bool prepare(const std::string& sourceUrl);

    // 开始播放
    void play();

    // 暂停播放
    void pause();

    // 精确跳转到指定时间
    void seekTo(int64_t timestampMs);

    // 获取当前播放进度
    int64_t getCurrentPosition() const;

    // 是否正在播放
    bool isPlaying() const { return m_isPlaying; }

public:
    void setProject(std::shared_ptr<core::NLEProject> project);
    void updatePlaybackProgress(); // Mock driver for testing

private:
    std::shared_ptr<core::NLEPreloadManager> m_preloadManager;
    bool m_isPlaying = false;
    int64_t m_currentPositionMs = 0;
    void* m_window = nullptr;

    std::shared_ptr<core::RenderThread> m_renderThread;
};

} // namespace modules
} // namespace video_sdk
