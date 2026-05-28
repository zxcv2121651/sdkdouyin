#pragma once
#include "../libavutil/avutil.h"

#define AV_PKT_FLAG_KEY 0x0001

struct AVPacket {
    int64_t pts;
    int64_t dts;
    uint8_t *data;
    int size;
    int stream_index;
    int flags;
};

struct AVCodec {
    const char *name;
    enum AVMediaType type;
};

struct AVCodecParameters {
    enum AVMediaType codec_type;
    uint32_t codec_id;
};

struct AVCodecContext {
    int width, height;
    int pix_fmt;
    int64_t bit_rate;
};

inline AVPacket *av_packet_alloc(void) { return new AVPacket{}; }
inline void av_packet_free(AVPacket **pkt) { delete *pkt; *pkt = nullptr; }
inline void av_packet_unref(AVPacket *pkt) {}

inline AVCodec *avcodec_find_decoder(uint32_t id) { return new AVCodec{}; }
inline AVCodec *avcodec_find_decoder_by_name(const char *name) { return new AVCodec{}; }
inline AVCodecContext *avcodec_alloc_context3(const AVCodec *codec) { return new AVCodecContext{}; }
inline int avcodec_parameters_to_context(AVCodecContext *codec, const AVCodecParameters *par) { return 0; }
inline int avcodec_open2(AVCodecContext *avctx, const AVCodec *codec, void **options) { return 0; }
inline void avcodec_free_context(AVCodecContext **avctx) { delete *avctx; *avctx = nullptr; }

inline int avcodec_send_packet(AVCodecContext *avctx, const AVPacket *avpkt) { return 0; }
inline int avcodec_receive_frame(AVCodecContext *avctx, AVFrame *frame) { return 0; }
