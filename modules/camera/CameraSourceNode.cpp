#include "CameraSourceNode.h"
#include <cstring>
#include <iostream>

namespace video_sdk {
namespace modules {

CameraSourceNode::CameraSourceNode(const std::string& id, std::shared_ptr<rhi::IRenderer> renderer)
    : RenderNode(id) {
    m_oesFilter = std::make_shared<core::OesToRgbFilter>(renderer);
    for(int i=0; i<16; i++) m_matrix[i] = (i%5 == 0) ? 1.0f : 0.0f; // Identity matrix
}

void CameraSourceNode::updateOESTexture(uint32_t textureId, int width, int height, int64_t timestampMs, const float* transformMatrix) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_oesTextureId = textureId;
    m_width = width;
    m_height = height;
    if (transformMatrix) {
        std::memcpy(m_matrix, transformMatrix, sizeof(float) * 16);
    }
    m_hasNewFrame = true;
}

void CameraSourceNode::process(core::RenderContext& context) {
    if (!m_outputTexture) return;

    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_hasNewFrame && m_oesTextureId > 0) {
        // Here we need to wrap the external texture ID into an RHI ITexture
        auto oesTex = context.renderer->wrapExternalOESTexture(m_oesTextureId, m_width, m_height);

        m_oesFilter->render(oesTex, m_outputTexture, m_matrix);
        // Do not set m_hasNewFrame = false here immediately if we need to repeatedly render the same frame (e.g., paused preview)
        // But for camera, it's a constant stream anyway.
    }
}

}
}
