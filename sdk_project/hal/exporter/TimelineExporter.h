#pragma once

#include "../interface/ITimelineExporter.h"
#include <atomic>
#include <thread>

namespace video_sdk {
namespace hal {

/**
 * @brief TimelineExporter 导出引擎实现。
 * 负责在后台新开线程、构建无头 (Headless) EGL/Metal Context，
 * 并驱动引擎以最大速度渲染每一帧，进而送入硬件编码器中。
 */
class TimelineExporter : public ITimelineExporter {
public:
    TimelineExporter();
    ~TimelineExporter() override;

    void startExport() override;
    void cancelExport() override;
    float getProgress() override;

private:
    void exportLoop();

private:
    std::thread m_exportThread;
    std::atomic<bool> m_isExporting{false};
    std::atomic<float> m_progress{0.0f};
};

} // namespace hal
} // namespace video_sdk
