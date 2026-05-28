#pragma once

#include <atomic>
#include <vector>
#include <cstddef>
#include <stdexcept>
#include <new>

namespace video_sdk {
namespace core {

// 工业级定义：缓存行对齐大小，一般 x86_64 / ARM64 都是 64 字节
// 用于防止 False Sharing (伪共享)，极大提升并发性能
#ifndef CACHE_LINE_SIZE
#define CACHE_LINE_SIZE 64
#endif

/**
 * @brief 工业级单生产者-单消费者 (SPSC) 无锁环形队列。
 * @tparam T 队列中存放的数据类型 (要求能够被默认构造和安全拷贝/移动)
 *
 * 核心设计：
 * 1. 采用 `std::atomic<size_t>` 配合 Acquire/Release 内存顺序。
 * 2. 读写索引被隔离在不同的 Cache Line 中，避免多 CPU 核心缓存颠簸 (False Sharing)。
 * 3. 队列容量固定，使用环形数组避免频繁 new/delete 造成的内存碎片和锁开销。
 */
template <typename T>
class LockFreeRingBuffer {
public:
    explicit LockFreeRingBuffer(size_t capacity) {
        if (capacity == 0) {
            throw std::invalid_argument("Capacity must be greater than 0");
        }
        // 为了区分队列满和空，实际数组大小需要 capacity + 1
        m_capacity = capacity + 1;
        m_buffer.resize(m_capacity);

        m_head.store(0, std::memory_order_relaxed);
        m_tail.store(0, std::memory_order_relaxed);
    }

    ~LockFreeRingBuffer() = default;

    // 禁用拷贝和赋值
    LockFreeRingBuffer(const LockFreeRingBuffer&) = delete;
    LockFreeRingBuffer& operator=(const LockFreeRingBuffer&) = delete;

    /**
     * @brief 生产者端：尝试将数据压入队列
     * @param item 要入队的数据
     * @return 成功返回 true；如果队列已满则返回 false (非阻塞)
     */
    bool push(const T& item) {
        // 使用 relaxed 内存序加载，因为单生产者线程中只有自己会修改 m_head
        size_t head = m_head.load(std::memory_order_relaxed);
        size_t nextHead = next(head);

        // 使用 acquire 内存序读取 m_tail，确保获取到消费者最新的消费进度
        if (nextHead == m_tail.load(std::memory_order_acquire)) {
            // 队列已满
            return false;
        }

        m_buffer[head] = item;

        // 使用 release 内存序更新 m_head，保证数据写入 m_buffer 对消费者可见
        m_head.store(nextHead, std::memory_order_release);
        return true;
    }

    /**
     * @brief 生产者端：尝试将数据以移动语义压入队列
     */
    bool push(T&& item) {
        size_t head = m_head.load(std::memory_order_relaxed);
        size_t nextHead = next(head);

        if (nextHead == m_tail.load(std::memory_order_acquire)) {
            return false;
        }

        m_buffer[head] = std::move(item);
        m_head.store(nextHead, std::memory_order_release);
        return true;
    }

    /**
     * @brief 消费者端：尝试从队列弹出数据
     * @param item 用于接收弹出的数据
     * @return 成功返回 true；如果队列为空则返回 false (非阻塞)
     */
    bool pop(T& item) {
        // 使用 relaxed 内存序加载，单消费者线程中只有自己修改 m_tail
        size_t tail = m_tail.load(std::memory_order_relaxed);

        // 使用 acquire 内存序读取 m_head，确保看到生产者写入的最新的数据
        if (tail == m_head.load(std::memory_order_acquire)) {
            // 队列为空
            return false;
        }

        item = std::move(m_buffer[tail]);

        // 使用 release 内存序更新 m_tail，通知生产者该槽位已被释放
        m_tail.store(next(tail), std::memory_order_release);
        return true;
    }

    /**
     * @brief 检查队列是否为空 (可能非绝对实时，通常在 pop() 失败时已包含此逻辑)
     */
    bool empty() const {
        return m_head.load(std::memory_order_acquire) == m_tail.load(std::memory_order_acquire);
    }

    /**
     * @brief 检查队列是否已满
     */
    bool full() const {
        size_t head = m_head.load(std::memory_order_acquire);
        size_t tail = m_tail.load(std::memory_order_acquire);
        return next(head) == tail;
    }

    /**
     * @brief 清空队列 (仅限在停止生产和消费时调用才安全)
     */
    void clear() {
        m_head.store(0, std::memory_order_relaxed);
        m_tail.store(0, std::memory_order_relaxed);
    }

private:
    size_t next(size_t current) const {
        return (current + 1) % m_capacity;
    }

private:
    size_t m_capacity;
    std::vector<T> m_buffer;

    // --- 对齐和填充，防止 False Sharing ---

    // 强制按缓存行对齐 (C++17 语法)
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> m_head;

    // 中间填充，确保 m_head 和 m_tail 绝对不在同一个缓存行
    char padding1[CACHE_LINE_SIZE - sizeof(std::atomic<size_t>)];

    alignas(CACHE_LINE_SIZE) std::atomic<size_t> m_tail;

    // 尾部填充，防止 m_tail 与相邻的其他对象处于同一缓存行
    char padding2[CACHE_LINE_SIZE - sizeof(std::atomic<size_t>)];
};

} // namespace core
} // namespace video_sdk
