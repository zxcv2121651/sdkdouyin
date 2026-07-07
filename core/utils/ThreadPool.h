#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>

namespace video_sdk {
namespace core {

/**
 * @brief 工业级轻量线程池
 * 用于 RenderGraph 的多节点并发渲染及其他重型 CPU 任务。
 */
class ThreadPool {
public:
    explicit ThreadPool(size_t threads);
    ~ThreadPool();

    // 投递任务，并返回 future 以便上层获取结果或等待完成
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::invoke_result<F, Args...>::type>;

private:
    // 工作线程列表
    std::vector<std::thread> workers;
    // 任务队列
    std::queue<std::function<void()>> tasks;

    // 同步原语
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> std::future<typename std::invoke_result<F, Args...>::type>
{
    using return_type = typename std::invoke_result<F, Args...>::type;

    auto task = std::make_shared< std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        // 如果线程池已停止，不允许继续投递
        if(stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace([task](){ (*task)(); });
    }
    condition.notify_one();
    return res;
}

} // namespace core
} // namespace video_sdk
