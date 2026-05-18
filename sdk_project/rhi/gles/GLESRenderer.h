#pragma once
#include "IRenderer.h"
#include <unordered_map>
#include <vector>
#include <string>

namespace video_sdk {
namespace rhi {

/**
 * @brief GLES 特性等级枚举，用于动态适配不同硬件能力
 */
enum class GLESVersion {
    GLES_2_0,
    GLES_3_0,
    GLES_3_1,
    GLES_3_2,
    UNKNOWN
};

/**
 * @brief OpenGL ES 的渲染器具体实现。
 * 包含动态版本探测、Shader 宏动态分发以及 FBO 缓存池。
 */
class GLESRenderer : public IRenderer {
public:
    GLESRenderer();
    ~GLESRenderer() override;

    void initialize() override;
    void destroy() override;

    uint32_t acquireFBO(int width, int height) override;
    void releaseFBO(uint32_t fboId) override;

    uint32_t compileShader(const std::string& vertexSource, const std::string& fragmentSource) override;

    // 获取当前探测到的 GLES 版本
    GLESVersion getVersion() const { return m_currentVersion; }

private:
    // 解析 GLES 版本字符串
    void detectCapability();

    // 根据当前 GLES 版本动态注入 Shader 宏
    std::string injectShaderMacros(const std::string& source, bool isVertexShader);

private:
    GLESVersion m_currentVersion = GLESVersion::UNKNOWN;

    // 统一的 FBO 缓存池：std::unordered_map 与轻量级 struct key O(1) 查找
    struct FBOKey {
        int width;
        int height;
        bool operator==(const FBOKey& other) const {
            return width == other.width && height == other.height;
        }
    };

    // Hash function for FBOKey
    struct FBOKeyHash {
        std::size_t operator()(const FBOKey& k) const {
            return std::hash<int>()(k.width) ^ (std::hash<int>()(k.height) << 1);
        }
    };

    std::unordered_map<FBOKey, std::vector<uint32_t>, FBOKeyHash> m_fboPool;
};

} // namespace rhi
} // namespace video_sdk
