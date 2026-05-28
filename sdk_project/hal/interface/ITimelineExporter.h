#pragma once

namespace video_sdk {
namespace hal {

/**
 * @brief ITimelineExporter 负责在独立的渲染上下文（Context）中处理离线视频导出逻辑，避免阻塞 UI 线程。
 */
class ITimelineExporter {
public:
    virtual ~ITimelineExporter() = default;

    // 开始导出
    virtual void startExport() = 0;
    // 取消导出
    virtual void cancelExport() = 0;
    // 获取当前导出进度
    virtual float getProgress() = 0;
};

} // namespace hal
} // namespace video_sdk
