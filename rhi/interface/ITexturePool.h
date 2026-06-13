#pragma once

#include "IRenderer.h"
#include <memory>

namespace video_sdk {
namespace rhi {

/**
 * @brief 商业级显存池化管理接口 (Texture / FBO Pooling)
 * 解决移动端每帧分配显存导致的内存碎片化和性能瓶颈。
 */
class ITexturePool {
public:
    virtual ~ITexturePool() = default;

    /**
     * @brief 从池中获取一个符合要求的纹理。如果缓存中没有，则请求 Renderer 创建一个新的。
     * @param width 宽度
     * @param height 高度
     * @param format 纹理格式
     * @return 返回复用或新分配的纹理
     */
    virtual std::shared_ptr<ITexture> acquireTexture(int width, int height, TextureFormat format) = 0;

    /**
     * @brief 用完纹理后将其归还给池子，使其变为“空闲”状态以供下一帧复用。
     * @param texture 要归还的纹理
     */
    virtual void recycleTexture(std::shared_ptr<ITexture> texture) = 0;

    /**
     * @brief 清空缓存池，释放所有底层 GPU 资源 (常用于进入后台或收到内存警告时)
     */
    virtual void purge() = 0;
};

} // namespace rhi
} // namespace video_sdk
