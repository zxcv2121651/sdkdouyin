#pragma once
#include "IRenderer.h"
#include <unordered_map>
#include <vector>

namespace video_sdk {
namespace rhi {

/**
 * @brief OpenGL ES 的渲染器具体实现。
 * 负责管理 GLES 上下文、Shader 程序，以及 FBO 缓存池。
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

private:
    // 统一的 FBO 缓存池：std::unordered_map 与轻量级 struct key O(1) 查找
    // 这里使用简单的结构表示池化
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
