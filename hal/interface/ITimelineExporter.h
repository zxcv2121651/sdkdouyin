#pragma once

#include <string>

namespace video_sdk {
namespace hal {

/**
 * @brief ITimelineExporter 导出引擎接口。
 */
class ITimelineExporter {
public:
    virtual ~ITimelineExporter() = default;

    /**
     * 开始后台导出流程
     * @param outputPath 输出文件的绝对路径。如果为空，将使用默认路径。
     */
    virtual void startExport(const std::string& outputPath = "") = 0;

    /**
     * 取消导出
     */
    virtual void cancelExport() = 0;

    /**
     * 获取导出进度 (0.0 ~ 1.0)
     */
    virtual float getProgress() = 0;
};

} // namespace hal
} // namespace video_sdk
