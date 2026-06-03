#pragma once
#include "../libavcodec/avcodec.h"

struct AVStream {
    int index;
    AVCodecParameters *codecpar;
};

struct AVFormatContext {
    unsigned int nb_streams;
    AVStream **streams;
    int64_t duration;
};

inline int avformat_open_input(AVFormatContext **ps, const char *url, void *fmt, void **options) {
    *ps = new AVFormatContext{};
    (*ps)->nb_streams = 2;
    (*ps)->streams = new AVStream*[2];

    (*ps)->streams[0] = new AVStream{0, new AVCodecParameters{AVMEDIA_TYPE_VIDEO, 27}}; // H264
    (*ps)->streams[1] = new AVStream{1, new AVCodecParameters{AVMEDIA_TYPE_AUDIO, 86018}}; // AAC
    return 0;
}
inline int avformat_find_stream_info(AVFormatContext *ic, void **options) { return 0; }
inline void avformat_close_input(AVFormatContext **s) {
    if (*s) {
        delete (*s)->streams[0]->codecpar;
        delete (*s)->streams[0];
        delete (*s)->streams[1]->codecpar;
        delete (*s)->streams[1];
        delete[] (*s)->streams;
        delete *s;
        *s = nullptr;
    }
}
inline int av_read_frame(AVFormatContext *s, AVPacket *pkt) { return AVERROR_EOF; }
inline int av_seek_frame(AVFormatContext *s, int stream_index, int64_t timestamp, int flags) { return 0; }

#define AVSEEK_FLAG_BACKWARD 1
