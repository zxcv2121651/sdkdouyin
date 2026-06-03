#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/**
 * 视频剪辑器的 Obj-C 门面（Facade）。
 * 作为 Swift 层与 C++ 核心层之间的桥梁。
 */
@interface VideoEditorIOS : NSObject

// 初始化编辑器实例
- (instancetype)init;

// 导入视频到时间线
- (BOOL)importVideo:(NSString *)filePath startTimeMs:(int64_t)startTimeMs error:(NSError **)error;

@end

NS_ASSUME_NONNULL_END
