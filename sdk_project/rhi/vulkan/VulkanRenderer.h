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
 * 对比 GLES 状态机模型，Vulkan 需要手动管理显存对象、预先构建 Pipeline (PSO)
 * 以及录制并提交 Command Buffer。性能极限提升 30% 以上。
 */
class VulkanRenderer : public IRenderer {
public:
    VulkanRenderer();
    ~VulkanRenderer() override;

    void initialize() override;
    void destroy() override;

    // Vulkan 中 FBO 池化意味着预先分配 VkImage, VkImageView 和 VkFramebuffer
    FrameBufferObject* acquireFBO(int width, int height) override;
    void releaseFBO(FrameBufferObject* fbo) override;

    uint32_t compileShader(const std::string& vertexSource, const std::string& fragmentSource) override;
    uint32_t compileComputeShader(const std::string& computeSource) override;
    void dispatchCompute(uint32_t programId, int numGroupsX, int numGroupsY, int numGroupsZ) override;

    // --- Vulkan 特定接口，用于 Node 进行底层绘制 ---

    // 获取当前活跃的 Command Buffer 进行录制
    VkCommandBuffer beginFrameCommandBuffer();
    // 提交 Command Buffer 到 Queue 执行
    void submitFrameCommandBuffer();

    // 根据 shaderProgramId 提取预先烘焙的 PSO
    VulkanPipeline getPipeline(uint32_t programId);

private:
    void initVulkanCore();
    void cleanupVulkanCore();
    FrameBufferObject* createVulkanFboInternal(int width, int height);

private:
    // Vulkan 核心设备抽象
    VkInstance m_instance = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;

    // 当前帧正在录制的 CommandBuffer
    VkCommandBuffer m_currentCmdBuffer = VK_NULL_HANDLE;

    // FBO 缓存池相关
    std::mutex m_fboMutex;
    struct FBOKey { int w, h; bool operator==(const FBOKey& o) const { return w == o.w && h == o.h; } };
    struct FBOKeyHash { size_t operator()(const FBOKey& k) const { return k.w ^ (k.h << 1); } };
    std::unordered_map<FBOKey, std::vector<FrameBufferObject*>, FBOKeyHash> m_fboPool;

    // Pipeline State Object 缓存
    std::mutex m_psoMutex;
    uint32_t m_shaderIdCounter = 1;
    std::unordered_map<uint32_t, VulkanPipeline> m_psoCache;
};

} // namespace rhi
} // namespace video_sdk
