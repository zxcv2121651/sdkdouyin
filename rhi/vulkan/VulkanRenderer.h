#pragma once
#include "rhi/interface/IRenderer.h"
#include <vulkan/vulkan.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <mutex>

namespace video_sdk {
namespace rhi {

/**
 * @brief Vulkan 独有的 Pipeline State Object (PSO) 缓存结构
 */
struct VulkanPipeline {
    VkPipeline pipeline = VK_NULL_HANDLE;
    VkPipelineLayout layout = VK_NULL_HANDLE;
};

/**
 * @brief 工业级 Vulkan 渲染后端实现。
 */
class VulkanRenderer : public IRenderer {
public:
    VulkanRenderer();
    ~VulkanRenderer() override;

    void initialize() override;
    void destroy() override;

    const RendererCapabilities& getCapabilities() const override { return m_caps; }

    FrameBufferObject* acquireFBO(int width, int height) override;
    void releaseFBO(FrameBufferObject* fbo) override;

    uint32_t compileShader(const std::string& vertexSource, const std::string& fragmentSource) override;
    uint32_t compileComputeShader(const std::string& computeSource) override;
    void dispatchCompute(uint32_t programId, int numGroupsX, int numGroupsY, int numGroupsZ) override;

    VkCommandBuffer beginFrameCommandBuffer();
    void submitFrameCommandBuffer();
    VulkanPipeline getPipeline(uint32_t programId);

private:
    void initVulkanCore();
    void cleanupVulkanCore();
    FrameBufferObject* createVulkanFboInternal(int width, int height);

private:
    RendererCapabilities m_caps;

    VkInstance m_instance = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;

    VkCommandBuffer m_currentCmdBuffer = VK_NULL_HANDLE;

    std::mutex m_fboMutex;
    struct FBOKey { int w, h; bool operator==(const FBOKey& o) const { return w == o.w && h == o.h; } };
    struct FBOKeyHash { size_t operator()(const FBOKey& k) const { return k.w ^ (k.h << 1); } };
    std::unordered_map<FBOKey, std::vector<FrameBufferObject*>, FBOKeyHash> m_fboPool;

    std::mutex m_psoMutex;
    uint32_t m_shaderIdCounter = 1;
    std::unordered_map<uint32_t, VulkanPipeline> m_psoCache;
};

} // namespace rhi
} // namespace video_sdk
