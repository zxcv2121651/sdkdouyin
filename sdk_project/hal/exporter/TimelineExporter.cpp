#include "TimelineExporter.h"
#include "hal/codec/AndroidMediaCodecEncoder.h"
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

    // 1. 初始化独立的后台 EGLCore 或 Metal Context (此处省略)
    // ...

    // 2. 初始化硬件编码器 (零拷贝模式)
    AndroidMediaCodecEncoder encoder;
    // 使用 HEVC (H.265), 1080p, 5Mbps, 30fps
    if (!encoder.initialize(173, 1080, 1920, 5000000, 30)) {
        std::cerr << "[TimelineExporter] Failed to initialize hardware encoder!" << std::endl;
        m_isExporting = false;
        return;
    }

    // 获取编码器表面，渲染引擎直接向其绘制
    void* inputSurface = encoder.getInputSurface();
    std::cout << "[TimelineExporter] Got InputSurface: " << inputSurface << std::endl;

    // 3. 极速渲染循环 (不再受 AVSyncClock 的真实时间限制)
    int totalFrames = 300; // 假设要导出 10 秒 30fps 的视频
    int currentFrame = 0;

    while (m_isExporting && currentFrame < totalFrames) {

        // 渲染管线执行 (伪代码)
        // eglMakeCurrent(display, inputSurface, inputSurface, context);
        // graph->render();
        // eglSwapBuffers(display, inputSurface);

        // 提交编码器排空
        encoder.drainOutput();

        // 模拟极速导出耗时 (硬件渲染通常每帧只需 3~5ms)
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        currentFrame++;
        m_progress = static_cast<float>(currentFrame) / totalFrames;

        if (currentFrame % 50 == 0) {
            std::cout << "[TimelineExporter] Progress: " << (m_progress * 100.0f) << "%" << std::endl;
        }
    }

    // 刷新剩余的编码缓冲
    encoder.drainOutput();
    encoder.destroy();

    if (m_isExporting) {
        std::cout << "[TimelineExporter] Export completed 100%!" << std::endl;
        m_progress = 1.0f;
        m_isExporting = false;
    } else {
        std::cout << "[TimelineExporter] Export CANCELED!" << std::endl;
    }

    // 4. 清理 Context (此处省略)
}

} // namespace hal
} // namespace video_sdk
