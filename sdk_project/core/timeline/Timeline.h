#pragma once
#include <vector>
#include <unordered_map>
#include <shared_mutex>
#include <memory>
#include <string>
#include "Clip.h"

namespace video_sdk {
namespace core {

/**
 * @brief Timeline 是非线性编辑（NLE）引擎的核心数据总线。
 * 它描述了整个剪辑工程的静态状态，包括所有的媒体轨道、视频片段(Clip)、音频和特效。
 *
 * 核心设计：
 * 1. 它是纯数据驱动的（Data-Driven），上层 UI 的任何拖拉拽最终都映射为修改 Timeline 内部的参数。
 * 2. 线程安全：因为 UI 线程会频繁修改时间轴，而后台的解码和渲染线程（RenderThread）会
 *    以 60FPS 的频率高频读取 Timeline 状态来决定画什么。因此内部使用了读写锁（shared_mutex）
 *    来保证极高的并发读取性能。
 */
class Timeline {
public:
    Timeline();
    ~Timeline();

    /**
     * @brief 将一个片段添加到指定的轨道上
     * @param trackId 轨道标识符（区分主视频轨、画中画轨、音频轨等）
     * @param clip 媒体片段实例
     */
    void addClip(const std::string& trackId, std::shared_ptr<Clip> clip);

    /**
     * @brief 核心查询接口：获取在给定的时间点下，需要参与渲染的所有片段。
     * 渲染引擎在绘制每一帧时都会调用此接口，找出当前屏幕上重叠的所有视频或图片。
     * @param timeMs 绝对时间戳 (毫秒)
     * @return 当前时间命中 (Hit) 的片段集合
     */
    std::vector<std::shared_ptr<Clip>> getClipsAtTime(int64_t timeMs);

    /**
     * @brief 移除特定的片段
     * @param clipId 目标片段的 ID
     */
    void removeClip(const std::string& clipId);

    /**
     * @brief 更新片段在时间轴上的区间位置（常用于用户在 UI 上拖拽改变时长）
     * @param clipId 目标片段 ID
     * @param newInMs 新的全局入点
     * @param newOutMs 新的全局出点
     */
    void updateClipRange(const std::string& clipId, int64_t newInMs, int64_t newOutMs);

    /**
     * @brief 根据 ID 获取 Clip 对象
     */
    std::shared_ptr<Clip> getClip(const std::string& clipId);

private:
    // 工业级并发控制：读写锁 (C++17 shared_mutex)
    // 允许渲染线程无阻塞并发读取，仅在 UI 修改时短暂独占锁住。
    mutable std::shared_mutex m_mutex;

    // 快速索引哈希表 (用于增删改查)
    std::unordered_map<std::string, std::shared_ptr<Clip>> m_clipMap;

    // 顺序存储，实际工程中通常嵌套为 vector<Track>，每个 Track 内包含 vector<Clip>
    std::vector<std::shared_ptr<Clip>> m_orderedClips;
};

} // namespace core
} // namespace video_sdk
