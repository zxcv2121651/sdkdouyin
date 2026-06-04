#include "CameraCapture.h"
#include <iostream>
#include <vector>

namespace video_sdk {
namespace modules {

/**
 * @brief Android 平台采集接收器。
 * 此时已经废弃了 NDK Camera2 的直接调用，
 * 只作为一个桥梁，接收来自 Java/Kotlin 层面的 OES 纹理 ID。
 */
class CameraCaptureAndroid : public ICameraCaptureImpl {
public:
    CameraCaptureAndroid() {}
    ~CameraCaptureAndroid() override {}

    void setPreviewWindow(void* window) override {
        m_externalWindow = window;
        std::cout << "[Camera OES] Bound external preview window: " << m_externalWindow << std::endl;
    }

    void pushOESTexture(uint32_t textureId, int width, int height, int64_t timestampMs, const float* transformMatrix) override {
        if (m_frameCallback) {
            // OES 纹理通常需要一个 Transform Matrix 才能正确显示（处理镜像、旋转）
            m_frameCallback(textureId, width, height, timestampMs, transformMatrix);
        }
    }

    void setFrameCallback(OnFrameAvailableCallback callback) override {
        m_frameCallback = callback;
    }

private:
    OnFrameAvailableCallback m_frameCallback;
    void* m_externalWindow = nullptr;
};

// 平台工厂实现
std::unique_ptr<ICameraCaptureImpl> CreatePlatformCameraImpl() {
    return std::make_unique<CameraCaptureAndroid>();
}

} // namespace modules
} // namespace video_sdk
