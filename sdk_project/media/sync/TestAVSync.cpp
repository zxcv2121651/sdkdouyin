#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include "AVSyncClock.h"
#include "RenderThreadSyncLoop.h"

using namespace video_sdk::media;

int main() {
    std::cout << "--- Starting LockFree A/V Sync Engine Test ---" << std::endl;

    auto masterClock = std::make_shared<AVSyncClock>();
    masterClock->setClock(0);

    // 设置 RingBuffer 容量为 3 (非常小，以便我们测试积压时的反压机制)
    RenderThreadSyncLoop renderLoop(masterClock, 3);
    renderLoop.start();

    // 模拟极高并发的解码线程推帧
    for (int i = 0; i < 6; ++i) {
        VideoFrame frame{};
        frame.pts = i * 33;

        // 尝试非阻塞的推入无锁队列
        bool success = false;
        int retries = 0;
        while (!success && retries < 5) {
            success = renderLoop.enqueueFrame(frame);
            if (!success) {
                std::cout << "[Decoder] RingBuffer FULL! Frame " << i << " blocked. Retrying..." << std::endl;
                // 反压机制 (Backpressure)：队列满时，解码器必须休眠等待渲染消耗
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                retries++;
            }
        }

        if(success) {
            std::cout << "[Decoder] Successfully pushed frame " << i << " PTS: " << frame.pts << "ms" << std::endl;
        } else {
            std::cout << "[Decoder] Failed to push frame " << i << " after retries. Dropping at decoder layer." << std::endl;
        }

        // 模拟解码极快，不休眠直接下一帧
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    renderLoop.stop();
    std::cout << "--- LockFree A/V Sync Engine Test Finished ---" << std::endl;
    return 0;
}
