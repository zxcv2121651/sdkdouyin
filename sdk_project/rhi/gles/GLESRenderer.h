#pragma once
#include "IRenderer.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <mutex>

namespace video_sdk {
namespace rhi {

enum class GLESVersion {
    GLES_2_0,
    GLES_3_0,
    GLES_3_1,
    GLES_3_2,
    UNKNOWN
};

/**
 * @brief OpenGL ES 渲染器实现。包含 FBO 池化及 Compute Shader 支持。
 */
class GLESRenderer : public IRenderer {
public:
    GLESRenderer();
    ~GLESRenderer() override;

    void initialize() override;
    void destroy() override;

    FrameBufferObject* acquireFBO(int width, int height) override;
    void releaseFBO(FrameBufferObject* fbo) override;

    uint32_t compileShader(const std::string& vertexSource, const std::string& fragmentSource) override;
    uint32_t compileComputeShader(const std::string& computeSource) override;
    void dispatchCompute(uint32_t programId, int numGroupsX, int numGroupsY, int numGroupsZ) override;

    GLESVersion getVersion() const { return m_currentVersion; }

private:
    void detectCapability();
    std::string injectShaderMacros(const std::string& source, bool isVertexShader);

    // 内部创建真实 FBO 的逻辑
    FrameBufferObject* createFBOInternal(int width, int height);

private:
    GLESVersion m_currentVersion = GLESVersion::UNKNOWN;
    std::mutex m_fboMutex;

    struct FBOKey {
        int width;
        int height;
        bool operator==(const FBOKey& other) const {
            return width == other.width && height == other.height;
        }
    };

    struct FBOKeyHash {
        std::size_t operator()(const FBOKey& k) const {
            return std::hash<int>()(k.width) ^ (std::hash<int>()(k.height) << 1);
        }
    };

    // FBO 缓冲池：Key 是宽高，Value 是可复用的 FBO 列表
    std::unordered_map<FBOKey, std::vector<std::unique_ptr<FrameBufferObject>>, FBOKeyHash> m_fboPool;
};

} // namespace rhi
} // namespace video_sdk
