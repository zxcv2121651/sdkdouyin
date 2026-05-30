#pragma once

#include "../interface/ITimelineExporter.h"
#include "hal/codec/AndroidMediaCodecEncoder.h"
#include "core/utils/MessageLoop.h"
#include <atomic>
#include <memory>

namespace video_sdk {
namespace hal {

/**
 * @brief TimelineExporter 导出引擎实现。
 * 核心升级：废弃暴力 while() 循环，改用 MessageLoop 驱动状态机。
 * 实现高性能无阻塞导出。
 */
class TimelineExporter : public ITimelineExporter {
public:
    TimelineExporter();
    ~TimelineExporter() override;

    void startExport() override;
    void cancelExport() override;
    float getProgress() override;

private:
    void doExportNextFrame();
    void finishExport();

private:
    core::MessageLoop m_messageLoop;
    std::atomic<bool> m_isExporting{false};
    std::atomic<float> m_progress{0.0f};

    std::unique_ptr<AndroidMediaCodecEncoder> m_encoder;
    int m_totalFrames = 300;
    int m_currentFrame = 0;
};

} // namespace hal
} // namespace video_sdk
