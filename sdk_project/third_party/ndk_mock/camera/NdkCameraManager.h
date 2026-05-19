#pragma once
#include <stdint.h>

#define ACAMERA_OK 0

typedef struct ACameraManager ACameraManager;
typedef struct ACameraIdList ACameraIdList;
typedef struct ACameraDevice ACameraDevice;
typedef struct ACameraCaptureSession ACameraCaptureSession;
typedef struct ACaptureSessionOutputContainer ACaptureSessionOutputContainer;
typedef struct ACaptureSessionOutput ACaptureSessionOutput;
typedef struct ACameraOutputTarget ACameraOutputTarget;
typedef struct ACaptureRequest ACaptureRequest;

typedef struct ACameraDevice_StateCallbacks {
    void* context;
    void (*onDisconnected)(void* context, ACameraDevice* device);
    void (*onError)(void* context, ACameraDevice* device, int error);
} ACameraDevice_StateCallbacks;

typedef struct ACameraCaptureSession_stateCallbacks {
    void* context;
    void (*onClosed)(void* context, ACameraCaptureSession* session);
    void (*onReady)(void* context, ACameraCaptureSession* session);
    void (*onActive)(void* context, ACameraCaptureSession* session);
} ACameraCaptureSession_stateCallbacks;

// 模拟窗口/Surface 句柄
typedef void* ANativeWindow;

inline ACameraManager* ACameraManager_create() { return (ACameraManager*)1; }
inline void ACameraManager_delete(ACameraManager* manager) {}

inline int ACameraManager_getCameraIdList(ACameraManager* manager, ACameraIdList** cameraIdList) { return ACAMERA_OK; }
inline void ACameraManager_deleteCameraIdList(ACameraIdList* cameraIdList) {}

inline int ACameraManager_openCamera(ACameraManager* manager, const char* cameraId, ACameraDevice_StateCallbacks* callback, ACameraDevice** device) { *device = (ACameraDevice*)2; return ACAMERA_OK; }
inline int ACameraDevice_close(ACameraDevice* device) { return ACAMERA_OK; }

inline int ACameraOutputTarget_create(ANativeWindow* window, ACameraOutputTarget** target) { *target = (ACameraOutputTarget*)3; return ACAMERA_OK; }
inline void ACameraOutputTarget_free(ACameraOutputTarget* target) {}

inline int ACaptureSessionOutput_create(ANativeWindow* window, ACaptureSessionOutput** output) { *output = (ACaptureSessionOutput*)4; return ACAMERA_OK; }
inline void ACaptureSessionOutput_free(ACaptureSessionOutput* output) {}

inline int ACaptureSessionOutputContainer_create(ACaptureSessionOutputContainer** container) { *container = (ACaptureSessionOutputContainer*)5; return ACAMERA_OK; }
inline void ACaptureSessionOutputContainer_free(ACaptureSessionOutputContainer* container) {}
inline int ACaptureSessionOutputContainer_add(ACaptureSessionOutputContainer* container, const ACaptureSessionOutput* output) { return ACAMERA_OK; }

inline int ACameraDevice_createCaptureSession(ACameraDevice* device, const ACaptureSessionOutputContainer* outputs, const ACameraCaptureSession_stateCallbacks* callbacks, ACameraCaptureSession** session) { *session = (ACameraCaptureSession*)6; return ACAMERA_OK; }
inline void ACameraCaptureSession_close(ACameraCaptureSession* session) {}

inline int ACameraDevice_createCaptureRequest(ACameraDevice* device, int templateId, ACaptureRequest** request) { *request = (ACaptureRequest*)7; return ACAMERA_OK; }
inline void ACaptureRequest_free(ACaptureRequest* request) {}
inline int ACaptureRequest_addTarget(ACaptureRequest* request, const ACameraOutputTarget* target) { return ACAMERA_OK; }
inline int ACameraCaptureSession_setRepeatingRequest(ACameraCaptureSession* session, ACameraCaptureSession_stateCallbacks* callbacks, int numRequests, ACaptureRequest** requests, int* captureSequenceId) { return ACAMERA_OK; }

#define TEMPLATE_PREVIEW 1
