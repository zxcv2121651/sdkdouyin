#pragma once
#include "rhi/interface/IRenderer.h"
#include <mutex>
#include <unordered_map>
#include <vector>
#include <list>
#include <memory>

namespace video_sdk {
namespace rhi {

class IFBOPool {
public:
    virtual ~IFBOPool() = default;
    virtual FrameBufferObject* acquireFBO(int width, int height) = 0;
    virtual void recycleFBO(FrameBufferObject* fbo) = 0;
    virtual void purge() = 0;
};

class DefaultFBOPool : public IFBOPool {
public:
    explicit DefaultFBOPool(std::shared_ptr<IRenderer> renderer);
    ~DefaultFBOPool() override;

    FrameBufferObject* acquireFBO(int width, int height) override;
    void recycleFBO(FrameBufferObject* fbo) override;
    void purge() override;

private:
    struct FBOKey {
        int width;
        int height;
        bool operator==(const FBOKey& other) const { return width == other.width && height == other.height; }
    };

    struct FBOKeyHash {
        std::size_t operator()(const FBOKey& k) const {
            return std::hash<int>()(k.width) ^ (std::hash<int>()(k.height) << 1);
        }
    };

    std::shared_ptr<IRenderer> m_renderer;
    std::mutex m_mutex;

    std::list<FrameBufferObject*> m_lruList;
    std::unordered_map<FBOKey, std::vector<FrameBufferObject*>, FBOKeyHash> m_freeFBOs;

    size_t m_maxPoolSize = 30;
};

}
}
