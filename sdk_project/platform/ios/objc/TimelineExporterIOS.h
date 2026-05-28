#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/**
 * TimelineExporterIOS 负责 iOS 平台的离线导出。
 * 它会在后台队列中创建独立的 EAGLContext，并通过 AVAssetWriterInputPixelBufferAdaptor 输出视频。
 */
@interface TimelineExporterIOS : NSObject

// 开始导出
- (void)startExport;
// 取消导出
- (void)cancelExport;
// 获取当前导出进度
- (float)getProgress;

@end

NS_ASSUME_NONNULL_END
