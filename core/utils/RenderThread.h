#pragma once

#include "MessageLoop.h"
#include "rhi/interface/IRenderer.h"
#include "rhi/RendererFactory.h"
#include <memory>
#include <future>

namespace video_sdk {
namespace core {

/**
 * @brief 统一渲染线程模型 (Unified Render Thread)
 * 商业 SDK 的核心防线。继承自 MessageLoop。
 * 所有涉及到 RHI (特别是 OpenGL/EGL) 的上下文创建、销毁和绘制指令，
 * 必须且只能通过此线程的内部任务队列执行。
 */
class RenderThread : public MessageLoop {
public:
    RenderThread();
    ~RenderThread();

    /**
     * @brief 启动渲染线程，并在该线程内部初始化 RHI 环境。
     */
    bool startRenderThread();

    /**
     * @brief 停止渲染线程，并在线程退出前销毁 RHI 环境。
     */
    void stopRenderThread();

    /**
     * @brief 获取绑定在此线程上的渲染器。
     * @warning 只能在投递到该线程的 Task 内部调用。
     */
    std::shared_ptr<rhi::IRenderer> getRenderer() const { return m_renderer; }

    /**
     * @brief 同步投递任务并等待返回结果 (用于需要立刻知道底层结果的 C-API)
     */
    template<typename F, typename... Args>
    auto postTaskAndReply(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type> {
        using return_type = typename std::invoke_result<F, Args...>::type;
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        std::future<return_type> res = task->get_future();
        this->postTask([task]() { (*task)(); });
        return res;
    }

private:
    std::shared_ptr<rhi::IRenderer> m_renderer;
};

} // namespace core
} // namespace video_sdk
