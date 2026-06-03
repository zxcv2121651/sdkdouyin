#pragma once
#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AMediaMuxer AMediaMuxer;
typedef struct AMediaFormat AMediaFormat;

typedef enum {
    AMEDIAMUXER_OUTPUT_FORMAT_MPEG_4 = 0,
    AMEDIAMUXER_OUTPUT_FORMAT_WEBM = 1,
} OutputFormat;

// 使用定义好的 AMediaCodecBufferInfo，包含在 NdkMediaCodec 中
#ifndef AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED
struct AMediaCodecBufferInfo;
#endif

inline AMediaMuxer* AMediaMuxer_new(int fd, OutputFormat format) { return (AMediaMuxer*)1; }
inline media_status_t AMediaMuxer_delete(AMediaMuxer* muxer) { return AMEDIA_OK; }
inline ssize_t AMediaMuxer_addTrack(AMediaMuxer* muxer, const AMediaFormat* format) { return 0; }
inline media_status_t AMediaMuxer_start(AMediaMuxer* muxer) { return AMEDIA_OK; }
inline media_status_t AMediaMuxer_stop(AMediaMuxer* muxer) { return AMEDIA_OK; }
inline media_status_t AMediaMuxer_writeSampleData(AMediaMuxer* muxer, size_t trackIdx, const uint8_t* data, const struct AMediaCodecBufferInfo* info) { return AMEDIA_OK; }

#ifdef __cplusplus
}
#endif
