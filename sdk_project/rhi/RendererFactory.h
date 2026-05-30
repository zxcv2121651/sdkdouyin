#pragma once
#include <memory>
#include "rhi/interface/IRenderer.h"

namespace video_sdk {
namespace rhi {

enum class Backend {
    DEFAULT = 0, // 自动探测，Vulkan 优先，GLES 保底
    OPENGLES,
    VULKAN,
    METAL
};

/**
 * @brief RHI 渲染器工厂，负责硬件能力的探测与最佳后端的选择。
 * 遵循 Google Filament 类似的动态加载与降级策略。
 */
class RendererFactory {
public:
    /**
     * 根据设备能力和请求的后端类型创建渲染器
     * @param requestedBackend 期望的后端类型
     * @return 实例化后的渲染器接口指针
     */
    static std::shared_ptr<IRenderer> createRenderer(Backend requestedBackend = Backend::DEFAULT);

private:
    static bool probeVulkanSupport();
};

} // namespace rhi
} // namespace video_sdk
