#include "MessageLoop.h"
#include <iostream>
#include <pthread.h>

namespace video_sdk {
namespace core {

MessageLoop::MessageLoop() {}

MessageLoop::~MessageLoop() {
    stop();
}

void MessageLoop::start(const std::string& threadName) {
    if (m_isRunning) return;

    m_isRunning = true;
    m_thread = std::thread([this, threadName]() {
#if defined(__APPLE__)
        pthread_setname_np(threadName.c_str());
#elif defined(__linux__) || defined(ANDROID)
        pthread_setname_np(pthread_self(), threadName.c_str());
#endif
        this->loop();
    });
}

void MessageLoop::stop() {
    if (!m_isRunning) return;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_isRunning = false;
    }
    m_cv.notify_all();

    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void MessageLoop::postTask(Task task) {
    if (!m_isRunning || !task) return;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_taskQueue.push(std::move(task));
    }
    m_cv.notify_one();
}

void MessageLoop::postDelayedTask(Task task, int64_t delayMs) {
    if (!m_isRunning || !task) return;

    int64_t executeTimeMs = getCurrentTimeMs() + delayMs;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_delayedTaskQueue.push({executeTimeMs, std::move(task)});
    }
    m_cv.notify_one();
}

int64_t MessageLoop::getCurrentTimeMs() const {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

void MessageLoop::loop() {
    while (m_isRunning) {
        Task currentTask = nullptr;

        {
            std::unique_lock<std::mutex> lock(m_mutex);

            // 1. 如果有立即执行的任务，直接取出
            if (!m_taskQueue.empty()) {
                currentTask = std::move(m_taskQueue.front());
                m_taskQueue.pop();
            }
            // 2. 如果没有立即任务，检查延时任务
            else {
                if (!m_delayedTaskQueue.empty()) {
                    int64_t nowMs = getCurrentTimeMs();
                    auto& topTask = m_delayedTaskQueue.top();

                    if (nowMs >= topTask.executeTimeMs) {
                        // 时间到了，取出执行
                        currentTask = std::move(topTask.task);
                        m_delayedTaskQueue.pop();
                    } else {
                        // 时间没到，休眠直到最近的一个任务到期
                        int64_t waitTimeMs = topTask.executeTimeMs - nowMs;
                        m_cv.wait_for(lock, std::chrono::milliseconds(waitTimeMs));
                    }
                } else {
                    // 3. 既没有立即任务，也没有延时任务，彻底挂起休眠，不吃任何 CPU
                    m_cv.wait(lock);
                }
            }
        }

        // 离开锁的范围执行任务，防止任务内部 postTask 造成死锁
        if (currentTask) {
            currentTask();
        }
    }

    // 退出时清空队列
    std::lock_guard<std::mutex> lock(m_mutex);
    while (!m_taskQueue.empty()) m_taskQueue.pop();
    while (!m_delayedTaskQueue.empty()) m_delayedTaskQueue.pop();
}

} // namespace core
} // namespace video_sdk
