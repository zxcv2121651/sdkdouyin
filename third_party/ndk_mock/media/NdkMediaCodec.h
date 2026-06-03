#pragma once
#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AMediaCodec AMediaCodec;
typedef struct AMediaFormat AMediaFormat;

typedef enum {
    AMEDIA_OK = 0,
    AMEDIA_ERROR_BASE = -10000,
    AMEDIA_ERROR_UNKNOWN = AMEDIA_ERROR_BASE,
} media_status_t;

struct AMediaCodecBufferInfo {
    int32_t offset;
    int32_t size;
    int64_t presentationTimeUs;
    uint32_t flags;
};

#define AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED -3
#define AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED -4
#define AMEDIACODEC_INFO_TRY_AGAIN_LATER -1

inline AMediaCodec* AMediaCodec_createDecoderByType(const char* mime) { return (AMediaCodec*)1; }
inline media_status_t AMediaCodec_configure(AMediaCodec* codec, const AMediaFormat* format, void* surface, void* crypto, uint32_t flags) { return AMEDIA_OK; }
inline media_status_t AMediaCodec_start(AMediaCodec* codec) { return AMEDIA_OK; }
inline media_status_t AMediaCodec_stop(AMediaCodec* codec) { return AMEDIA_OK; }
inline media_status_t AMediaCodec_delete(AMediaCodec* codec) { return AMEDIA_OK; }
inline ssize_t AMediaCodec_dequeueInputBuffer(AMediaCodec* codec, int64_t timeoutUs) { return 0; }
inline uint8_t* AMediaCodec_getInputBuffer(AMediaCodec* codec, size_t idx, size_t* out_size) { if(out_size) *out_size = 1024; static uint8_t buf[1024]; return buf; }
inline media_status_t AMediaCodec_queueInputBuffer(AMediaCodec* codec, size_t idx, off_t offset, size_t size, uint64_t time, uint32_t flags) { return AMEDIA_OK; }
inline ssize_t AMediaCodec_dequeueOutputBuffer(AMediaCodec* codec, AMediaCodecBufferInfo* info, int64_t timeoutUs) { return 0; }
inline media_status_t AMediaCodec_releaseOutputBuffer(AMediaCodec* codec, size_t idx, bool render) { return AMEDIA_OK; }

inline AMediaCodec* AMediaCodec_createEncoderByType(const char* mime) { return (AMediaCodec*)1; }
inline media_status_t AMediaCodec_createInputSurface(AMediaCodec* codec, void** surface) { return AMEDIA_OK; }

#ifdef __cplusplus
}
#endif

inline uint8_t* AMediaCodec_getOutputBuffer(AMediaCodec* codec, size_t idx, size_t* out_size) { if(out_size) *out_size = 1024; static uint8_t buf[1024]; return buf; }

inline AMediaFormat* AMediaCodec_getOutputFormat(AMediaCodec* codec) { return (AMediaFormat*)1; }
