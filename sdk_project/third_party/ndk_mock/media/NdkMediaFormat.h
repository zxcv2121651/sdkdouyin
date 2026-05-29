#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AMediaFormat AMediaFormat;

extern const char* AMEDIAFORMAT_KEY_MIME;
extern const char* AMEDIAFORMAT_KEY_WIDTH;
extern const char* AMEDIAFORMAT_KEY_HEIGHT;
extern const char* AMEDIAFORMAT_KEY_BIT_RATE;
extern const char* AMEDIAFORMAT_KEY_FRAME_RATE;
extern const char* AMEDIAFORMAT_KEY_I_FRAME_INTERVAL;
extern const char* AMEDIAFORMAT_KEY_COLOR_FORMAT;

inline AMediaFormat* AMediaFormat_new() { return (AMediaFormat*)1; }
inline media_status_t AMediaFormat_delete(AMediaFormat* format) { return AMEDIA_OK; }
inline void AMediaFormat_setString(AMediaFormat* format, const char* name, const char* value) {}
inline void AMediaFormat_setInt32(AMediaFormat* format, const char* name, int32_t value) {}

#ifdef __cplusplus
}
#endif
