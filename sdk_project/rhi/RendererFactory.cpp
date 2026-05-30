#include "RendererFactory.h"
#include <iostream>

#ifdef ANDROID
#include <dlfcn.h>
#endif

// 包含具体实现类
#include "gles/GLESRenderer.h"
#include "vulkan/VulkanRenderer.h"

namespace video_sdk {
namespace rhi {

bool RendererFactory::probeVulkanSupport() {
#ifdef ANDROID
    // 工业级做法：不硬链接 vulkan.so，使用 dlopen 动态试探，防止在老机器上 Crash
    void* libvulkan = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
    if (libvulkan) {
        // 获取 vkGetInstanceProcAddr，确保库不仅能加载而且是有效版本的 Vulkan
        void* getProcAddr = dlsym(libvulkan, "vkGetInstanceProcAddr");
        dlclose(libvulkan);

        if (getProcAddr) {
            std::cout << "[RendererFactory] Vulkan is supported on this device." << std::endl;
            return true;
        }
    }
    std::cout << "[RendererFactory] Vulkan is NOT supported on this device." << std::endl;
    return false;
#else
    // Mock for other platforms / tests
    return true;
#endif
}

std::shared_ptr<IRenderer> RendererFactory::createRenderer(Backend requestedBackend) {
    if (requestedBackend == Backend::DEFAULT || requestedBackend == Backend::VULKAN) {
        // 尝试探测 Vulkan 能力
        if (probeVulkanSupport()) {
            std::cout << "[RendererFactory] Creating VulkanRenderer." << std::endl;
            return std::make_shared<VulkanRenderer>();
        } else if (requestedBackend == Backend::VULKAN) {
            // 如果明确要求 Vulkan 但不支持，报错或强制返回 null
            std::cerr << "[RendererFactory] Failed to create VulkanRenderer. Device does not support it." << std::endl;
            return nullptr;
        }
    }

    // 降级 (Fallback) 或明确要求 OPENGLES
    std::cout << "[RendererFactory] Creating GLESRenderer (Fallback to OpenGL ES)." << std::endl;
    return std::make_shared<GLESRenderer>();
}

} // namespace rhi
} // namespace video_sdk
