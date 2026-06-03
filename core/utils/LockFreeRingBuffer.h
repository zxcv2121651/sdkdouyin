#pragma once

#include <atomic>
#include <vector>
#include <cstddef>
#include <stdexcept>
#include <new>

namespace video_sdk {
namespace core {

#ifndef CACHE_LINE_SIZE
#define CACHE_LINE_SIZE 64
#endif

template <typename T>
class LockFreeRingBuffer {
public:
    explicit LockFreeRingBuffer(size_t capacity) {
        if (capacity == 0) {
            throw std::invalid_argument("Capacity must be greater than 0");
        }
        m_capacity = capacity + 1;
        m_buffer.resize(m_capacity);

        m_head.store(0, std::memory_order_relaxed);
        m_tail.store(0, std::memory_order_relaxed);
    }

    ~LockFreeRingBuffer() = default;

    LockFreeRingBuffer(const LockFreeRingBuffer&) = delete;
    LockFreeRingBuffer& operator=(const LockFreeRingBuffer&) = delete;

    bool push(const T& item) {
        size_t head = m_head.load(std::memory_order_relaxed);
        size_t nextHead = next(head);

        if (nextHead == m_tail.load(std::memory_order_acquire)) {
            return false;
        }

        m_buffer[head] = item;

        m_head.store(nextHead, std::memory_order_release);
        return true;
    }

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

    bool pop(T& item) {
        size_t tail = m_tail.load(std::memory_order_relaxed);

        if (tail == m_head.load(std::memory_order_acquire)) {
            return false;
        }

        item = std::move(m_buffer[tail]);

        m_tail.store(next(tail), std::memory_order_release);
        return true;
    }

    bool pop() {
        size_t tail = m_tail.load(std::memory_order_relaxed);
        if (tail == m_head.load(std::memory_order_acquire)) {
            return false;
        }
        m_tail.store(next(tail), std::memory_order_release);
        return true;
    }

    bool peek(T& item) {
        size_t tail = m_tail.load(std::memory_order_relaxed);
        if (tail == m_head.load(std::memory_order_acquire)) {
            return false;
        }
        item = m_buffer[tail];
        return true;
    }

    bool empty() const {
        return m_head.load(std::memory_order_acquire) == m_tail.load(std::memory_order_acquire);
    }

    bool full() const {
        size_t head = m_head.load(std::memory_order_acquire);
        size_t tail = m_tail.load(std::memory_order_acquire);
        return next(head) == tail;
    }

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

    alignas(CACHE_LINE_SIZE) std::atomic<size_t> m_head;
    char padding1[CACHE_LINE_SIZE - sizeof(std::atomic<size_t>)];

    alignas(CACHE_LINE_SIZE) std::atomic<size_t> m_tail;
    char padding2[CACHE_LINE_SIZE - sizeof(std::atomic<size_t>)];
};

} // namespace core
} // namespace video_sdk
