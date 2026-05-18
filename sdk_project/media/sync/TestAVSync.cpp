#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include "AVSyncClock.h"
#include "RenderThreadSyncLoop.h"

using namespace video_sdk::media;

int main() {
    std::cout << "--- Starting A/V Sync Engine Test ---" << std::endl;

    // 1. 初始化主时钟 (以 0 为起点)
    auto masterClock = std::make_shared<AVSyncClock>();
    masterClock->setClock(0);

    // 2. 初始化并启动渲染线程
    RenderThreadSyncLoop renderLoop(masterClock);
    renderLoop.start();

    // 3. 模拟视频解码线程，以固定的速度产生帧
    // 假设 30FPS，每帧应间隔约 33.3ms
    for (int i = 0; i < 5; ++i) {
        VideoFrame frame{};
        frame.pts = i * 33; // 视频的时间戳: 0, 33, 66, 99, 132

        if (i == 2) {
            // 模拟解码器卡顿导致第3帧严重滞后 (主时钟已经跑到100ms+了，但解码出来的依然是 66ms 的视频)
            std::cout << "[Decoder] Simulating heavy lag for frame " << i << "..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } else if (i == 4) {
            // 模拟解码器超前解码 (主时钟还没到，视频先解出来了)
            std::cout << "[Decoder] Decoder is too fast for frame " << i << "..." << std::endl;
            // 不 sleep，直接喂给渲染队列
        } else {
            // 正常速度解码
            std::this_thread::sleep_for(std::chrono::milliseconds(33));
        }

        std::cout << "[Decoder] Pushing frame " << i << " with PTS: " << frame.pts << "ms" << std::endl;
        renderLoop.enqueueFrame(frame);
    }

    // 等待渲染队列处理完毕
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    renderLoop.stop();
    std::cout << "--- A/V Sync Engine Test Finished ---" << std::endl;
    return 0;
}
