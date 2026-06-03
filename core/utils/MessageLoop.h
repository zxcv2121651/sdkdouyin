#pragma once
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <chrono>
#include <string>

namespace video_sdk {
namespace core {

/**
 * @brief 工业级的事件驱动消息循环 (Looper/TaskRunner 模型)。
 * 完美替代暴力的 while(true) + sleep 轮询模型，实现零功耗休眠与精准定时唤醒。
 * 允许在任意线程向目标线程 (通常是渲染线程) 投递闭包任务。
 */
class MessageLoop {
public:
    using Task = std::function<void()>;

    MessageLoop();
    ~MessageLoop();

    // 启动消息循环 (将在新线程中运行 loop)
    void start(const std::string& threadName = "MessageLoopThread");

    // 停止循环并等待线程退出
    void stop();

    // 投递一个立即执行的任务
    void postTask(Task task);

    // 投递一个延时任务 (毫秒)
    void postDelayedTask(Task task, int64_t delayMs);

    // 检查是否正在运行
    bool isRunning() const { return m_isRunning; }

private:
    struct DelayedTask {
        int64_t executeTimeMs;
        Task task;

        bool operator>(const DelayedTask& other) const {
            return executeTimeMs > other.executeTimeMs;
        }
    };

    void loop();
    int64_t getCurrentTimeMs() const;

private:
    std::thread m_thread;
    std::atomic<bool> m_isRunning{false};

    std::mutex m_mutex;
    std::condition_variable m_cv;

    std::queue<Task> m_taskQueue;
    std::priority_queue<DelayedTask, std::vector<DelayedTask>, std::greater<DelayedTask>> m_delayedTaskQueue;
};

} // namespace core
} // namespace video_sdk
