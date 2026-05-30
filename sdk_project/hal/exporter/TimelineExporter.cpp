#include "TimelineExporter.h"
#include <iostream>

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
    m_currentFrame = 0;
    m_totalFrames = 300; // 假设导出10秒30帧

    m_messageLoop.start("ExportThread");

    // 抛出初始化任务到后台线程
    m_messageLoop.postTask([this]() {
        std::cout << "[TimelineExporter] Starting offline export on MessageLoop..." << std::endl;

        m_encoder = std::make_unique<AndroidMediaCodecEncoder>();
        if (!m_encoder->initialize(173, 1080, 1920, 5000000, 30)) {
            std::cerr << "[TimelineExporter] Failed to init hardware encoder!" << std::endl;
            m_isExporting = false;
            return;
        }

        std::cout << "[TimelineExporter] Got InputSurface: " << m_encoder->getInputSurface() << std::endl;

        // 启动第一帧渲染
        this->doExportNextFrame();
    });
}

void TimelineExporter::cancelExport() {
    if (!m_isExporting) return;

    // 发送取消指令到队列
    m_messageLoop.postTask([this]() {
        if (m_isExporting) {
            std::cout << "[TimelineExporter] Export CANCELED!" << std::endl;
            this->finishExport();
        }
    });

    m_messageLoop.stop();
}

float TimelineExporter::getProgress() {
    return m_progress.load();
}

void TimelineExporter::doExportNextFrame() {
    if (!m_isExporting) return;

    if (m_currentFrame >= m_totalFrames) {
        std::cout << "[TimelineExporter] Export completed 100%!" << std::endl;
        m_progress = 1.0f;
        finishExport();
        return;
    }

    // 1. 模拟执行 RenderGraph (伪代码: graph->render())
    // 2. 模拟从 GPU 获取编码缓冲
    m_encoder->drainOutput();

    m_currentFrame++;
    m_progress = static_cast<float>(m_currentFrame) / m_totalFrames;

    if (m_currentFrame % 50 == 0) {
        std::cout << "[TimelineExporter] Progress: " << (m_progress * 100.0f) << "%" << std::endl;
    }

    // 利用 MessageLoop 模拟硬件渲染的时间，不阻塞 CPU
    // 实际工业级代码中：这里应该是等待 EGL/Vulkan 的 Fence/Sync 回调后再调度下一帧
    m_messageLoop.postDelayedTask([this]() {
        this->doExportNextFrame();
    }, 5); // 模拟5ms耗时
}

void TimelineExporter::finishExport() {
    if (m_encoder) {
        m_encoder->drainOutput(); // 冲刷剩余数据
        m_encoder->destroy();
        m_encoder.reset();
    }
    m_isExporting = false;
}

} // namespace hal
} // namespace video_sdk
