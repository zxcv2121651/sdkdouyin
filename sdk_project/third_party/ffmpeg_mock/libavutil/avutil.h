#pragma once
#include <stdint.h>

#define AVERROR_EOF (-541478725)
#define AVERROR(e) (-(e))
#define EAGAIN 11

enum AVMediaType {
    AVMEDIA_TYPE_UNKNOWN = -1,
    AVMEDIA_TYPE_VIDEO,
    AVMEDIA_TYPE_AUDIO,
    AVMEDIA_TYPE_DATA,
    AVMEDIA_TYPE_SUBTITLE,
    AVMEDIA_TYPE_ATTACHMENT,
    AVMEDIA_TYPE_NB
};

struct AVFrame {
    uint8_t *data[8];
    int linesize[8];
    int width, height;
    int format;
    int64_t pts;
    int64_t pkt_dts;
};

inline AVFrame *av_frame_alloc(void) { return new AVFrame{}; }
inline void av_frame_free(AVFrame **frame) { delete *frame; *frame = nullptr; }
