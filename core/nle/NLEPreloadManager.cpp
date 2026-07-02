#include "NLEPreloadManager.h"
#include <iostream>
#include <cmath>

namespace video_sdk {
namespace core {

NLEPreloadManager::NLEPreloadManager(std::shared_ptr<hal::DecoderPool> decoderPool)
    : m_decoderPool(std::move(decoderPool)) {
}

NLEPreloadManager::~NLEPreloadManager() {
}

void NLEPreloadManager::setProject(std::shared_ptr<NLEProject> project) {
    m_project = std::move(project);
    m_lastPtsMs = -1;
}

void NLEPreloadManager::updatePlayhead(int64_t currentPtsMs) {
    if (!m_project || !m_decoderPool) return;

    // 避免过高频率的无意义刷新，比如同一毫秒内多次调用，或者时间倒流（除非明确的 Seek）
    if (m_lastPtsMs >= 0 && std::abs(currentPtsMs - m_lastPtsMs) < 30) {
        return; // 控制触发精度，约一两帧的间隔
    }

    processSlotsForTime(currentPtsMs, false);
    m_lastPtsMs = currentPtsMs;
}

void NLEPreloadManager::onSeek(int64_t seekPtsMs) {
    if (!m_project || !m_decoderPool) return;

    // Seek 操作意味着整个时间轴的连续性被打破，必须强制重新评估所有 Slot 的状态
    processSlotsForTime(seekPtsMs, true);
    m_lastPtsMs = seekPtsMs;
}

void NLEPreloadManager::processSlotsForTime(int64_t currentPtsMs, bool forceRefresh) {
    for (const auto& track : m_project->tracks) {
        if (track->type != NLETrackType::VIDEO) {
            continue; // 目前只关心视频轨道（需要解码器）
        }

        for (const auto& slot : track->slots) {
            int64_t slotStart = slot->timelineStartTimeMs;
            int64_t slotEnd = slotStart + slot->sourceDurationMs; // 简化：不考虑变速情况

            // 1. 判断是否处于【预读窗口】内： 即将播放（未来 PRELOAD_WINDOW_MS 毫秒内）
            bool isUpcoming = (slotStart > currentPtsMs && slotStart <= currentPtsMs + PRELOAD_WINDOW_MS);

            // 2. 判断是否处于【正在播放】状态
            bool isPlaying = (currentPtsMs >= slotStart && currentPtsMs <= slotEnd);

            // 3. 判断是否已经【彻底播放完毕且过期】
            bool isExpired = (currentPtsMs > slotEnd + EVICT_WINDOW_MS);

            // 如果处于即将播放 或 正在播放，我们需要确保解码器是 Pin 且 Ready 的
            if (isUpcoming || isPlaying) {
                if (isUpcoming) {
                    // 如果是即将播放，触发异步预读。
                    // 假设 CodecId 27 (H264), 默认不绑定 Surface 进行纯解码
                    // 在真实的商业引擎中，这里还需要根据 NLE 模型获取素材的宽高和真实的 Codec 类型
                    // std::cout << "[NLEPreloadManager] Preloading asset: " << slot->resourcePath << std::endl;
                    m_decoderPool->preloadHardwareDecoderAsync(slot->resourcePath, 27, 1920, 1080, nullptr);
                }

                // 只要在这个窗口内，就必须 Pin 住，防止被 LRU 干掉
                m_decoderPool->pinDecoder(slot->resourcePath);
            }
            // 如果已经过期，或者因为 Seek 导致距离当前播放头极远
            else if (isExpired || (slotStart > currentPtsMs + PRELOAD_WINDOW_MS && forceRefresh)) {
                // 已经播放完了，或者被 Seek 抛弃了，解除 Pin 状态，让 DecoderPool 的 LRU 可以将其回收
                // std::cout << "[NLEPreloadManager] Unpinning asset: " << slot->resourcePath << std::endl;
                m_decoderPool->unpinDecoder(slot->resourcePath);
            }
        }
    }
}

} // namespace core
} // namespace video_sdk
