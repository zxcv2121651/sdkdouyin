#pragma once
#include <stdint.h>

#define VK_SUCCESS 0
#define VK_NULL_HANDLE 0

// Vulkan 对象句柄 (使用指针模拟)
typedef void* VkInstance;
typedef void* VkPhysicalDevice;
typedef void* VkDevice;
typedef void* VkQueue;
typedef void* VkCommandPool;
typedef void* VkCommandBuffer;
typedef void* VkImage;
typedef void* VkImageView;
typedef void* VkFramebuffer;
typedef void* VkRenderPass;
typedef void* VkPipelineLayout;
typedef void* VkPipeline;
typedef void* VkShaderModule;
typedef void* VkDescriptorSetLayout;
typedef void* VkDescriptorPool;
typedef void* VkDescriptorSet;

typedef uint32_t VkFlags;
typedef uint32_t VkResult;

enum VkStructureType {
    VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO = 14,
    VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO = 37,
    VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO = 28,
    VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO = 40,
    VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO = 42,
    VK_STRUCTURE_TYPE_SUBMIT_INFO = 4,
    VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO = 43
};

enum VkPipelineBindPoint { VK_PIPELINE_BIND_POINT_GRAPHICS = 0 };
enum VkFormat { VK_FORMAT_R8G8B8A8_UNORM = 37 };
enum VkExtent3D { };

struct VkExtent2D { uint32_t width; uint32_t height; };
struct VkOffset2D { int32_t x; int32_t y; };
struct VkRect2D { VkOffset2D offset; VkExtent2D extent; };

// 模拟的占位结构体
struct VkImageCreateInfo { VkStructureType sType; };
struct VkFramebufferCreateInfo { VkStructureType sType; };
struct VkGraphicsPipelineCreateInfo { VkStructureType sType; };
struct VkCommandBufferAllocateInfo { VkStructureType sType; };
struct VkCommandBufferBeginInfo { VkStructureType sType; };
struct VkSubmitInfo { VkStructureType sType; };
struct VkRenderPassBeginInfo { VkStructureType sType; };
struct VkAllocationCallbacks { };

// Vulkan C API Mocks
inline VkResult vkCreateImage(VkDevice device, const VkImageCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImage* pImage) { *pImage = (VkImage)1; return VK_SUCCESS; }
inline void vkDestroyImage(VkDevice device, VkImage image, const VkAllocationCallbacks* pAllocator) {}

inline VkResult vkCreateFramebuffer(VkDevice device, const VkFramebufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFramebuffer* pFramebuffer) { *pFramebuffer = (VkFramebuffer)1; return VK_SUCCESS; }
inline void vkDestroyFramebuffer(VkDevice device, VkFramebuffer framebuffer, const VkAllocationCallbacks* pAllocator) {}

inline VkResult vkCreateGraphicsPipelines(VkDevice device, void* pipelineCache, uint32_t createInfoCount, const VkGraphicsPipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines) { *pPipelines = (VkPipeline)1; return VK_SUCCESS; }
inline void vkDestroyPipeline(VkDevice device, VkPipeline pipeline, const VkAllocationCallbacks* pAllocator) {}

inline VkResult vkAllocateCommandBuffers(VkDevice device, const VkCommandBufferAllocateInfo* pAllocateInfo, VkCommandBuffer* pCommandBuffers) { *pCommandBuffers = (VkCommandBuffer)1; return VK_SUCCESS; }
inline void vkFreeCommandBuffers(VkDevice device, VkCommandPool commandPool, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers) {}

inline VkResult vkBeginCommandBuffer(VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo* pBeginInfo) { return VK_SUCCESS; }
inline VkResult vkEndCommandBuffer(VkCommandBuffer commandBuffer) { return VK_SUCCESS; }

inline void vkCmdBeginRenderPass(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, int contents) {}
inline void vkCmdEndRenderPass(VkCommandBuffer commandBuffer) {}

inline void vkCmdBindPipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline) {}
inline void vkCmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {}

inline VkResult vkQueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo* pSubmits, void* fence) { return VK_SUCCESS; }
inline VkResult vkQueueWaitIdle(VkQueue queue) { return VK_SUCCESS; }
