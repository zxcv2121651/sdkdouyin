#pragma once
#include <memory>
#include <vector>
#include "rhi/interface/IRenderer.h"

namespace video_sdk {
namespace core {

/**
 * @brief Android 平台专属的 Zero-Copy 纹理转换滤镜。
 * 作用：将相机/硬件解码器生成的 GL_TEXTURE_EXTERNAL_OES 转换为 GL_TEXTURE_2D。
 * 同时负责应用 SurfaceTexture 的 transform matrix (处理镜像、倒转等硬件坐标系畸变)。
 */
class OesToRgbFilter {
public:
    OesToRgbFilter(std::shared_ptr<rhi::IRenderer> renderer);
    ~OesToRgbFilter();

    /**
     * @brief 执行 OES 转 2D 纹理的离屏渲染。
     * @param oesTextureId  外部硬件传来的 OES 纹理 (例如 SurfaceTexture.getTextureId)
     * @param fbo           目标帧缓冲对象 (内部包含一个标准的 GL_TEXTURE_2D)
     * @param matrix        16位浮点数组的仿射变换矩阵 (SurfaceTexture.getTransformMatrix)
     */
    void render(uint32_t oesTextureId, rhi::FrameBufferObject* fbo, const float* matrix);

private:
    void initShader();

private:
    std::shared_ptr<rhi::IRenderer> m_renderer;
    uint32_t m_programId = 0;

    // Shader 内部 Uniform 定位
    int m_matrixLoc = -1;
    int m_textureLoc = -1;
    int m_posLoc = -1;
    int m_coordLoc = -1;
};

} // namespace core
} // namespace video_sdk
