#pragma once

#include "rhi/interface/IRenderer.h"
#include <unordered_map>
#include <vector>
#include <mutex>
#include "rhi/gles/EGLCore.h"

namespace video_sdk {
namespace rhi {

/**
 * @brief OpenGL ES 渲染后端实现。
 * 结合 EGLCore 提供跨平台的 GLES 状态机管理与 FBO 对象池。
 */
class GLESRenderer : public IRenderer {
public:
    GLESRenderer();
    ~GLESRenderer() override;

    void initialize() override;
    void destroy() override;

    const RendererCapabilities& getCapabilities() const override { return m_caps; }

    FrameBufferObject* acquireFBO(int width, int height) override;
    void releaseFBO(FrameBufferObject* fbo) override;

    uint32_t compileShader(const std::string& vertexSource, const std::string& fragmentSource) override;
    uint32_t compileComputeShader(const std::string& computeSource) override;
    void dispatchCompute(uint32_t programId, int numGroupsX, int numGroupsY, int numGroupsZ) override;

private:
    void probeCapabilities();

private:
    RendererCapabilities m_caps;
    EGLCore m_eglCore;

    // FBO 缓存池：避免每帧频繁 glGenFramebuffers / glGenTextures 导致显存碎片化
    std::mutex m_fboMutex;

    // key=width_height, value=vector of FBOs
    struct FBOKey { int w, h; bool operator==(const FBOKey& o) const { return w == o.w && h == o.h; } };
    struct FBOKeyHash { size_t operator()(const FBOKey& k) const { return k.w ^ (k.h << 1); } };
    std::unordered_map<FBOKey, std::vector<FrameBufferObject*>, FBOKeyHash> m_fboPool;
};

} // namespace rhi
} // namespace video_sdk
