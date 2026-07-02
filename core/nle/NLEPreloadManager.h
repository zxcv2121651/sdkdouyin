#pragma once
#include <memory>
#include <vector>
#include <cstdint>
#include "core/nle/NLEModel.h"
#include "hal/interface/IDecoderPool.h"
#include "hal/codec/DecoderPool.h"

namespace video_sdk {
namespace core {

/**
 * @brief 商业级 NLE 预读管理器 (Preload Manager)
 * 核心职责：将上层的非线性编辑时间轴 (NLE Timeline) 与底层的硬件资源池 (DecoderPool) 桥接。
 * 通过监听当前播放进度，前瞻性地提前 2~3 秒在后台预加载即将播放的视频片段硬件解码器，
 * 并及时释放（Unpin）已经播放完毕的解码器。
 */
class NLEPreloadManager {
public:
    explicit NLEPreloadManager(std::shared_ptr<hal::DecoderPool> decoderPool);
    ~NLEPreloadManager();

    // 绑定当前的 NLE 项目草稿模型
    void setProject(std::shared_ptr<NLEProject> project);

    // 播放器高频驱动接口，传入当前播放时间戳(毫秒)
    // 内部将根据时间戳，计算哪些 Slot 即将播放，哪些已经过期
    void updatePlayhead(int64_t currentPtsMs);

    // 当用户在时间轴上快速拖拽 (Seek) 时调用，强制刷新缓存状态
    void onSeek(int64_t seekPtsMs);

private:
    std::shared_ptr<hal::DecoderPool> m_decoderPool;
    std::shared_ptr<NLEProject> m_project;

    // 预读窗口大小 (毫秒) - 例如：提前 2000ms 加载
    const int64_t PRELOAD_WINDOW_MS = 2000;

    // 过期窗口大小 (毫秒) - 例如：播放过当前时间点 500ms 后释放
    const int64_t EVICT_WINDOW_MS = 500;

    int64_t m_lastPtsMs = -1;

    void processSlotsForTime(int64_t currentPtsMs, bool forceRefresh);
};

} // namespace core
} // namespace video_sdk
