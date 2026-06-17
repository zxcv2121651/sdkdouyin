#include "../../include/video_sdk_c_api.h"
#include "modules/player/VideoPlayer.h"
#include "modules/camera/CameraCapture.h"
#include <memory>
#include <iostream>

struct VS_Player_Opaque {
    std::shared_ptr<video_sdk::modules::VideoPlayer> instance;
};

struct VS_Camera_Opaque {
    std::shared_ptr<video_sdk::modules::CameraCapture> instance;
};

VS_Result vs_global_initialize() {
    try {
        std::cout << "[VideoSDK C-API] Global Initialized." << std::endl;
        return VS_SUCCESS;
    } catch (...) {
        return VS_ERROR_INITIALIZATION_FAILED;
    }
}

void vs_global_destroy() {
    std::cout << "[VideoSDK C-API] Global Destroyed." << std::endl;
}

VS_Result vs_player_create(VS_PlayerHandle* out_handle) {
    if (!out_handle) return VS_ERROR_INVALID_ARGUMENT;
    try {
        auto player = std::make_shared<video_sdk::modules::VideoPlayer>();
        VS_Player_Opaque* opaque = new VS_Player_Opaque{player};
        *out_handle = opaque;
        return VS_SUCCESS;
    } catch (...) {
        return VS_ERROR_ENGINE_INTERNAL;
    }
}

void vs_player_destroy(VS_PlayerHandle handle) {
    if (handle) {
        delete handle;
    }
}

VS_Result vs_player_set_window(VS_PlayerHandle handle, void* window) {
    if (!handle || !handle->instance) return VS_ERROR_INVALID_HANDLE;
    try {
        handle->instance->setWindow(window);
        return VS_SUCCESS;
    } catch (...) {
        return VS_ERROR_ENGINE_INTERNAL;
    }
}

VS_Result vs_player_play(VS_PlayerHandle handle) {
    if (!handle || !handle->instance) return VS_ERROR_INVALID_HANDLE;
    try {
        handle->instance->play();
        return VS_SUCCESS;
    } catch (...) {
        return VS_ERROR_ENGINE_INTERNAL;
    }
}

VS_Result vs_player_pause(VS_PlayerHandle handle) {
    if (!handle || !handle->instance) return VS_ERROR_INVALID_HANDLE;
    try {
        handle->instance->pause();
        return VS_SUCCESS;
    } catch (...) {
        return VS_ERROR_ENGINE_INTERNAL;
    }
}

VS_Result vs_player_seek(VS_PlayerHandle handle, int64_t time_ms) {
    if (!handle || !handle->instance) return VS_ERROR_INVALID_HANDLE;
    try {
        handle->instance->seekTo(time_ms);
        return VS_SUCCESS;
    } catch (...) {
        return VS_ERROR_ENGINE_INTERNAL;
    }
}

VS_Result vs_camera_create(VS_CameraHandle* out_handle) {
    if (!out_handle) return VS_ERROR_INVALID_ARGUMENT;
    try {
        auto camera = std::make_shared<video_sdk::modules::CameraCapture>();
        VS_Camera_Opaque* opaque = new VS_Camera_Opaque{camera};
        *out_handle = opaque;
        return VS_SUCCESS;
    } catch (...) {
        return VS_ERROR_ENGINE_INTERNAL;
    }
}

void vs_camera_destroy(VS_CameraHandle handle) {
    if (handle) {
        delete handle;
    }
}

VS_Result vs_camera_set_preview_window(VS_CameraHandle handle, void* window) {
    if (!handle || !handle->instance) return VS_ERROR_INVALID_HANDLE;
    try {
        handle->instance->setPreviewWindow(window);
        return VS_SUCCESS;
    } catch (...) {
        return VS_ERROR_ENGINE_INTERNAL;
    }
}

VS_Result vs_camera_push_oes_texture(VS_CameraHandle handle, uint32_t texture_id, int width, int height, int64_t timestamp_ns, const float* transform_matrix) {
    if (!handle || !handle->instance) return VS_ERROR_INVALID_HANDLE;
    try {
        int64_t timestamp_ms = timestamp_ns / 1000000;
        handle->instance->pushOESTexture(texture_id, width, height, timestamp_ms, transform_matrix);
        return VS_SUCCESS;
    } catch (...) {
        return VS_ERROR_ENGINE_INTERNAL;
    }
}
