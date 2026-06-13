#include "VulkanRenderer.h"
#include <iostream>

namespace video_sdk {
namespace rhi {

VulkanRenderer::VulkanRenderer() {}
VulkanRenderer::~VulkanRenderer() {
    destroy();
}

void VulkanRenderer::initialize() {
    initVulkanCore();

    // Vulkan natively supports Compute Shaders
    m_caps.supportsComputeShader = true;
    m_caps.supportsImageLoadStore = true;
    m_caps.maxTextureSize = 4096;
}

void VulkanRenderer::initVulkanCore() {
    std::cout << "[Vulkan] Initializing Vulkan Logical Device & Queues..." << std::endl;
    // 工业级实现需调用 vkCreateInstance, vkCreateDevice, vkGetDeviceQueue 等
    // 此处简化为句柄赋值
    m_instance = (VkInstance)1;
    m_device = (VkDevice)2;
    m_graphicsQueue = (VkQueue)3;
    m_commandPool = (VkCommandPool)4;
}

void VulkanRenderer::cleanupVulkanCore() {
    // 销毁 Device 等
}

void VulkanRenderer::destroy() {
    std::lock_guard<std::mutex> lock(m_fboMutex);

    // 清理 FBO 相关的 VkImage 与 VkFramebuffer
    for (auto& pair : m_fboPool) {
        for (auto fbo : pair.second) {
            vkDestroyFramebuffer(m_device, (VkFramebuffer)(uintptr_t)fbo->fboId, nullptr);
            vkDestroyImage(m_device, (VkImage)(uintptr_t)fbo->textureId, nullptr);
            delete fbo;
        }
    }
    m_fboPool.clear();

    // 清理 Pipeline State Objects (PSOs)
    for (auto& pair : m_psoCache) {
        vkDestroyPipeline(m_device, pair.second.pipeline, nullptr);
    }
    m_psoCache.clear();

    cleanupVulkanCore();
}

FrameBufferObject* VulkanRenderer::createVulkanFboInternal(int width, int height) {
    // 1. 创建 VkImage 替代 GLES 的纹理
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    VkImage image = VK_NULL_HANDLE;
    vkCreateImage(m_device, &imageInfo, nullptr, &image);

    // 2. 创建 VkFramebuffer
    VkFramebufferCreateInfo fboInfo{};
    fboInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    VkFramebuffer vkFbo = VK_NULL_HANDLE;
    vkCreateFramebuffer(m_device, &fboInfo, nullptr, &vkFbo);

    auto fbo = new FrameBufferObject();
    fbo->width = width;
    fbo->height = height;

    // 危险强转演示：工业级代码中 FrameBufferObject 应包含 VkImage 和 VkFramebuffer 的专属字段
    // 这里为了复用 IRenderer 的 uint32_t，在 64 位系统上截断可能不安全，但在 Demo mock 环境可行
    fbo->textureId = (uint32_t)(uintptr_t)image;
    fbo->fboId = (uint32_t)(uintptr_t)vkFbo;

    return fbo;
}

FrameBufferObject* VulkanRenderer::acquireFBO(int width, int height) {
    std::lock_guard<std::mutex> lock(m_fboMutex);
    FBOKey key{width, height};

    if (!m_fboPool[key].empty()) {
        auto fbo = m_fboPool[key].back();
        m_fboPool[key].pop_back();
        return fbo;
    }

    return createVulkanFboInternal(width, height);
}

void VulkanRenderer::releaseFBO(FrameBufferObject* fbo) {
    if (!fbo) return;
    std::lock_guard<std::mutex> lock(m_fboMutex);
    m_fboPool[{fbo->width, fbo->height}].push_back(fbo);
}

uint32_t VulkanRenderer::compileShader(const std::string& vertexSource, const std::string& fragmentSource) {
    std::lock_guard<std::mutex> lock(m_psoMutex);
    uint32_t programId = m_shaderIdCounter++;

    // Vulkan 中编译 Shader(SPIR-V) 仅仅是第一步，真正的坑在于预构建 VkPipeline (PSO)
    // 工业级 SDK 在这必须读取一个预编译的 Pipeline Cache，极大地缩短应用冷启动时间
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    VkPipeline pipeline = VK_NULL_HANDLE;
    vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);

    VulkanPipeline pso;
    pso.pipeline = pipeline;
    // pso.layout = ...;

    m_psoCache[programId] = pso;
    return programId;
}

uint32_t VulkanRenderer::compileComputeShader(const std::string& computeSource) {
    // 类似编译 Graphics Pipeline，但是是 Compute Pipeline
    return m_shaderIdCounter++;
}

void VulkanRenderer::dispatchCompute(uint32_t programId, int numGroupsX, int numGroupsY, int numGroupsZ) {
    // Vulkan 中触发 Compute Shader:
    // vkCmdBindPipeline(m_currentCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pso.pipeline);
    // vkCmdDispatch(m_currentCmdBuffer, numGroupsX, numGroupsY, numGroupsZ);
}

VkCommandBuffer VulkanRenderer::beginFrameCommandBuffer() {
    if (m_currentCmdBuffer != VK_NULL_HANDLE) {
        return m_currentCmdBuffer;
    }

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    vkAllocateCommandBuffers(m_device, &allocInfo, &m_currentCmdBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    vkBeginCommandBuffer(m_currentCmdBuffer, &beginInfo);

    return m_currentCmdBuffer;
}

void VulkanRenderer::submitFrameCommandBuffer() {
    if (m_currentCmdBuffer == VK_NULL_HANDLE) return;

    vkEndCommandBuffer(m_currentCmdBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    // ... 配置 CommandBuffer, Wait Semaphores, Signal Semaphores

    vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);

    // 工业级做法不应直接 WaitIdle，而应利用 Fence 异步检查完成度以提升吞吐量
    vkQueueWaitIdle(m_graphicsQueue);

    // 释放重置 (由于使用了 CommandPool，可以直接 Reset 而不是 Free)
    // vkFreeCommandBuffers(...)
    m_currentCmdBuffer = VK_NULL_HANDLE;
}

VulkanPipeline VulkanRenderer::getPipeline(uint32_t programId) {
    std::lock_guard<std::mutex> lock(m_psoMutex);
    if (m_psoCache.find(programId) != m_psoCache.end()) {
        return m_psoCache[programId];
    }
    return VulkanPipeline{};
}

} // namespace rhi
} // namespace video_sdk
