#pragma once
#include "core/engine/RenderNode.h"
#include "core/engine/OesToRgbFilter.h"
#include <mutex>
#include <memory>
#include <atomic>

namespace video_sdk {
namespace modules {

/**
 * @brief 接收从 Kotlin 传入的 OES 纹理，并将其转换到 RenderGraph 的 Node.
 */
class CameraSourceNode : public core::RenderNode {
public:
    CameraSourceNode(const std::string& id, std::shared_ptr<rhi::IRenderer> renderer);
    ~CameraSourceNode() override = default;

    void process(core::RenderContext& context) override;

    // Kotlin 接收到相机帧后调用这个更新状态
    void updateOESTexture(uint32_t textureId, int width, int height, int64_t timestampMs, const float* transformMatrix);

private:
    std::shared_ptr<core::OesToRgbFilter> m_oesFilter;

    std::mutex m_mutex;
    uint32_t m_oesTextureId = 0;
    float m_matrix[16];
    int m_width = 0;
    int m_height = 0;
    std::atomic<bool> m_hasNewFrame{false};
};

}
}
