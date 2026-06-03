#pragma once
#include <stdint.h>
typedef struct AImageReader AImageReader;
typedef struct AImage AImage;
typedef void* ANativeWindow;

typedef struct AImageReader_ImageListener {
    void* context;
    void (*onImageAvailable)(void* context, AImageReader* reader);
} AImageReader_ImageListener;

inline int AImageReader_new(int32_t width, int32_t height, int32_t format, int32_t maxImages, AImageReader** reader) { *reader = (AImageReader*)1; return 0; }
inline void AImageReader_delete(AImageReader* reader) {}
inline int AImageReader_setImageListener(AImageReader* reader, AImageReader_ImageListener* listener) { return 0; }
inline int AImageReader_getWindow(AImageReader* reader, ANativeWindow** window) { *window = (ANativeWindow*)1; return 0; }
inline int AImageReader_acquireNextImage(AImageReader* reader, AImage** image) { *image = (AImage*)1; return 0; }
inline void AImage_delete(AImage* image) {}
