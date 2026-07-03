#pragma once
#include <mutex>
#include <vector>
#include <memory>

namespace video_sdk {
namespace core {

enum class MemoryPressureLevel {
    MODERATE = 0, // 系统内存吃紧，建议清理部分不重要的缓存 (如 Android TRIM_MEMORY_RUNNING_MODERATE)
    CRITICAL = 1  // 系统内存极其紧张，必须立即释放所有空闲资源 (如 Android TRIM_MEMORY_RUNNING_CRITICAL)
};

/**
 * @brief 内存压力监听器接口
 * 凡是占用大量显存/内存的资源池 (如 FBOPool, DecoderPool)，都应实现此接口。
 */
class IMemoryPressureListener {
public:
    virtual ~IMemoryPressureListener() = default;

    // 收到内存压力告警的回调
    virtual void onMemoryPressure(MemoryPressureLevel level) = 0;
};

/**
 * @brief 全局内存管理器
 * 接收来自外部操作系统 (如 Android onTrimMemory / iOS didReceiveMemoryWarning) 的告警，
 * 并同步分发给引擎内所有注册的资源池。
 */
class MemoryManager {
public:
    static MemoryManager& getInstance();

    // 注册监听器
    void addListener(std::weak_ptr<IMemoryPressureListener> listener);

    // 移除监听器 (可选，但通过 weak_ptr 可自动失效)
    void removeListener(std::shared_ptr<IMemoryPressureListener> listener);

    // 触发全局内存告警
    void notifyMemoryPressure(MemoryPressureLevel level);

private:
    MemoryManager() = default;
    ~MemoryManager() = default;

    MemoryManager(const MemoryManager&) = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;

    std::mutex m_mutex;
    std::vector<std::weak_ptr<IMemoryPressureListener>> m_listeners;
};

} // namespace core
} // namespace video_sdk
