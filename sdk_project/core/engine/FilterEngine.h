#pragma once
#include <memory>
#include <string>
#include <unordered_map>

namespace video_sdk {
namespace core {

/**
 * @brief FilterEngine 负责管理 100% 的特效渲染逻辑。
 * 运行在 RHI（渲染硬件接口）之上，尽可能确保零拷贝（Zero-Copy）。
 */
class FilterEngine {
public:
    FilterEngine();
    ~FilterEngine();

    // 应用特效滤镜
    void applyFilter(const std::string& filterName, uint32_t inputTextureId, uint32_t outputTextureId);

private:
    // 管理 Shader 状态及 FBO（帧缓冲对象）缓存池
};

} // namespace core
} // namespace video_sdk
