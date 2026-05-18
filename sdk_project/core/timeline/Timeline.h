#pragma once
#include <vector>
#include <unordered_map>
#include <shared_mutex>
#include <memory>
#include <string>

namespace video_sdk {
namespace core {

class Clip; // 前置声明

/**
 * @brief Timeline 是核心的非编（NLE）引擎组件。
 * 负责管理轨道（tracks）、片段（clips）及整体状态。
 */
class Timeline {
public:
    Timeline();
    ~Timeline();

    // 向时间线添加片段
    void addClip(const std::string& trackId, std::shared_ptr<Clip> clip);

    // 获取特定时间重叠的所有片段（高效查询）
    std::vector<std::shared_ptr<Clip>> getClipsAtTime(int64_t timeMs);

private:
    // 共享互斥锁，用于读写锁分离，保证线程安全
    mutable std::shared_mutex m_mutex;

    // 通过 ID 进行 O(1) 查找的哈希表
    std::unordered_map<std::string, std::shared_ptr<Clip>> m_clipMap;

    // 按 timelineIn（时间线入点）排序，用于高效的区间相交检测
    std::vector<std::shared_ptr<Clip>> m_orderedClips;
};

} // namespace core
} // namespace video_sdk
