#include "utils/RenderThread.h"
#include <iostream>
#include <chrono>

using namespace video_sdk::core;

int main() {
    std::cout << "--- Testing Unified RenderThread ---" << std::endl;

    RenderThread renderThread;

    std::cout << "[MainThread] Starting RenderThread..." << std::endl;
    bool success = renderThread.startRenderThread();
    if (!success) {
        std::cerr << "[MainThread] Failed to start RenderThread!" << std::endl;
        return 1;
    }

    std::cout << "[MainThread] Posting generic async tasks..." << std::endl;
    renderThread.postTask([]() {
        std::cout << "[RenderThread] Executing generic rendering task 1." << std::endl;
    });

    renderThread.postTask([]() {
        std::cout << "[RenderThread] Executing generic rendering task 2." << std::endl;
    });

    std::cout << "[MainThread] Posting delayed task..." << std::endl;
    renderThread.postDelayedTask([]() {
        std::cout << "[RenderThread] Executing delayed task (e.g. timeout logic)." << std::endl;
    }, 500);

    std::cout << "[MainThread] Posting sync task and waiting for reply..." << std::endl;
    auto futureResult = renderThread.postTaskAndReply([]() -> int {
        std::cout << "[RenderThread] Executing sync task (e.g. readPixels) and returning value." << std::endl;
        return 42;
    });

    int result = futureResult.get();
    std::cout << "[MainThread] Got result from RenderThread: " << result << std::endl;

    // Wait a bit to let the delayed task run
    std::this_thread::sleep_for(std::chrono::milliseconds(600));

    std::cout << "[MainThread] Stopping RenderThread..." << std::endl;
    renderThread.stopRenderThread();

    std::cout << "--- Testing Completed ---" << std::endl;
    return 0;
}
