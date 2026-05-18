#pragma once
#include <vector>
#include <unordered_map>
#include <shared_mutex>
#include <memory>
#include <string>

namespace video_sdk {
namespace core {

class Clip; // Forward declaration

/**
 * @brief Timeline is the core NLE (Non-Linear Editing) engine component.
 * It manages tracks, clips, and overall state.
 */
class Timeline {
public:
    Timeline();
    ~Timeline();

    // Add a clip to the timeline
    void addClip(const std::string& trackId, std::shared_ptr<Clip> clip);

    // Get clips overlapping a specific time (efficient query)
    std::vector<std::shared_ptr<Clip>> getClipsAtTime(int64_t timeMs);

private:
    // Shared mutex for reader-writer locking
    mutable std::shared_mutex m_mutex;

    // O(1) lookup by ID
    std::unordered_map<std::string, std::shared_ptr<Clip>> m_clipMap;

    // Ordered by timelineIn for efficient intersection tests
    std::vector<std::shared_ptr<Clip>> m_orderedClips;
};

} // namespace core
} // namespace video_sdk
