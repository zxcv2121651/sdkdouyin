#include "TimelineExporter.h"
#include <iostream>
#include <chrono>

namespace video_sdk {
namespace hal {

TimelineExporter::TimelineExporter() {}

TimelineExporter::~TimelineExporter() {
    cancelExport();
}

void TimelineExporter::startExport() {
    if (m_isExporting) return;

    m_isExporting = true;
    m_progress = 0.0f;
    m_exportThread = std::thread(&TimelineExporter::exportLoop, this);
}

void TimelineExporter::cancelExport() {
    if (!m_isExporting) return;
    m_isExporting = false;
    if (m_exportThread.joinable()) {
        m_exportThread.join();
    }
}

float TimelineExporter::getProgress() {
    return m_progress.load();
}

void TimelineExporter::exportLoop() {
    std::cout << "[TimelineExporter] Starting offline export..." << std::endl;

    // 1. 初始化独立的后台 EGLCore 或 Metal Context
    // ...

    // 2. 初始化硬件编码器 (如 Android MediaCodec Encoder)
    // ...

    // 3. 极速渲染循环 (不再受 AVSyncClock 的真实时间限制)
    int totalFrames = 300; // 假设要导出 10 秒 30fps 的视频
    int currentFrame = 0;

    while (m_isExporting && currentFrame < totalFrames) {

        // 渲染管线执行
        // graph->render(context);

        // 提交编码器
        // encoder->drainOutput();

        // 模拟极速导出耗时 (硬件渲染通常每帧只需 3~5ms)
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        currentFrame++;
        m_progress = static_cast<float>(currentFrame) / totalFrames;

        if (currentFrame % 50 == 0) {
            std::cout << "[TimelineExporter] Progress: " << (m_progress * 100.0f) << "%" << std::endl;
        }
    }

    if (m_isExporting) {
        std::cout << "[TimelineExporter] Export completed 100%!" << std::endl;
        m_progress = 1.0f;
        m_isExporting = false;
    } else {
        std::cout << "[TimelineExporter] Export CANCELED!" << std::endl;
    }

    // 4. 清理 Context 和 编码器
}

} // namespace hal
} // namespace video_sdk
