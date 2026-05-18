#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/**
 * TimelineExporterIOS manages the offline export using EAGLContext
 * and AVAssetWriterInputPixelBufferAdaptor.
 */
@interface TimelineExporterIOS : NSObject

- (void)startExport;
- (void)cancelExport;
- (float)getProgress;

@end

NS_ASSUME_NONNULL_END
