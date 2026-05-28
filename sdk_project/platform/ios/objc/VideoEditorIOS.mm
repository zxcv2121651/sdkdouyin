#import "VideoEditorIOS.h"
#include <memory>
#include <string>
#include <exception>
#include "modules/editor/VideoEditor.h"

// NSError 域常量
static NSString *const VideoSDKErrorDomain = @"com.video.sdk.error";

@interface VideoEditorIOS () {
    // 隐藏 C++ 指针，避免污染公开头文件
    std::shared_ptr<video_sdk::modules::VideoEditor> _editor;
}
@end

@implementation VideoEditorIOS

- (instancetype)init {
    self = [super init];
    if (self) {
        try {
            _editor = std::make_shared<video_sdk::modules::VideoEditor>();
        } catch (const std::exception& e) {
            NSLog(@"Failed to initialize VideoEditor: %s", e.what());
            return nil;
        }
    }
    return self;
}

- (BOOL)importVideo:(NSString *)filePath startTimeMs:(int64_t)startTimeMs error:(NSError **)error {
    if (!_editor) {
        if (error) {
            *error = [NSError errorWithDomain:VideoSDKErrorDomain code:1001 userInfo:@{NSLocalizedDescriptionKey: @"Editor instance is null"}];
        }
        return NO;
    }

    try {
        std::string path([filePath UTF8String]);
        _editor->importVideo(path, startTimeMs);
        return YES;
    } catch (const std::exception& e) {
        if (error) {
            NSString *errMsg = [NSString stringWithUTF8String:e.what()];
            *error = [NSError errorWithDomain:VideoSDKErrorDomain code:1002 userInfo:@{NSLocalizedDescriptionKey: errMsg}];
        }
        return NO;
    }
}

@end
