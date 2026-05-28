#pragma once
#include <vector>
#include <unordered_map>
#include <shared_mutex>
#include <memory>
#include <string>
#include "Clip.h"

namespace video_sdk {
namespace core {

class Timeline {
public:
    Timeline();
    ~Timeline();

    void addClip(const std::string& trackId, std::shared_ptr<Clip> clip);
    std::vector<std::shared_ptr<Clip>> getClipsAtTime(int64_t timeMs);
    void removeClip(const std::string& clipId);
    void updateClipRange(const std::string& clipId, int64_t newInMs, int64_t newOutMs);
    std::shared_ptr<Clip> getClip(const std::string& clipId);

private:
    mutable std::shared_mutex m_mutex;
    std::unordered_map<std::string, std::shared_ptr<Clip>> m_clipMap;
    std::vector<std::shared_ptr<Clip>> m_orderedClips;
};

} // namespace core
} // namespace video_sdk
