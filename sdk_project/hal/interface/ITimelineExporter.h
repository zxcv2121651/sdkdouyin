#pragma once

namespace video_sdk {
namespace hal {

/**
 * @brief ITimelineExporter handles off-screen exporting in a separate context.
 */
class ITimelineExporter {
public:
    virtual ~ITimelineExporter() = default;

    virtual void startExport() = 0;
    virtual void cancelExport() = 0;
    virtual float getProgress() = 0;
};

} // namespace hal
} // namespace video_sdk
