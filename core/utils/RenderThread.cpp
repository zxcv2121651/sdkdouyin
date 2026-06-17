#include "RenderThread.h"
#include <iostream>

namespace video_sdk {
namespace core {

RenderThread::RenderThread() {}

RenderThread::~RenderThread() {
    stopRenderThread();
}

bool RenderThread::startRenderThread() {
    // 使用 std::promise 同步等待初始化结果
    std::promise<bool> initPromise;
    auto initFuture = initPromise.get_future();

    // 1. 启动基础的消息循环线程
    this->start("VSDK_RenderThread");

    // 2. 投递一个任务到该线程，执行 RHI (EGL Context) 的初始化
    this->postTask([this, &initPromise]() {
        try {
            m_renderer = rhi::RendererFactory::createRenderer();
            if (m_renderer) {
                // OpenGL 强制要求 Context 的 create 和 makeCurrent 必须在这个 Thread 内
                m_renderer->initialize();
                initPromise.set_value(true);
            } else {
                initPromise.set_value(false);
            }
        } catch (...) {
            initPromise.set_value(false);
        }
    });

    // 3. 阻塞等待初始化完成
    bool success = initFuture.get();
    if (success) {
        std::cout << "[RenderThread] Render context initialized successfully on dedicated thread." << std::endl;
    } else {
        std::cerr << "[RenderThread] Failed to initialize render context." << std::endl;
        this->stop();
    }
    return success;
}

void RenderThread::stopRenderThread() {
    if (this->isRunning() && m_renderer) {
        // 同步等待销毁任务完成，保证 Context 销毁在正确的线程
        this->postTaskAndReply([this]() {
            std::cout << "[RenderThread] Tearing down render context..." << std::endl;
            m_renderer->destroy();
            m_renderer.reset();
        }).wait();
    }
    this->stop();
}

} // namespace core
} // namespace video_sdk
