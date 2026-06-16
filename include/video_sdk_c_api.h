#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// Video SDK C-API (ABI Stable Layer)
// 商业级 SDK 标准导出层：使用 Opaque Pointers (不透明指针) 彻底隐藏 C++ 细节，
// 保证跨编译器、跨标准库的二进制兼容性，并防止 C++ 异常抛出到外部环境。
// ============================================================================

// --- 错误码定义 ---
typedef enum {
    VS_SUCCESS = 0,
    VS_ERROR_INVALID_HANDLE = -1,
    VS_ERROR_INITIALIZATION_FAILED = -2,
    VS_ERROR_INVALID_ARGUMENT = -3,
    VS_ERROR_ENGINE_INTERNAL = -4
} VS_Result;

// --- 不透明句柄 (Opaque Pointers) ---
typedef struct VS_Engine_Opaque* VS_EngineHandle;
typedef struct VS_Player_Opaque* VS_PlayerHandle;
typedef struct VS_Camera_Opaque* VS_CameraHandle;
typedef struct VS_Timeline_Opaque* VS_TimelineHandle;

// ============================================================================
// 全局环境 API
// ============================================================================
VS_Result vs_global_initialize();
void vs_global_destroy();

// ============================================================================
// Engine (核心渲染引擎) API
// ============================================================================
VS_Result vs_engine_create(VS_EngineHandle* out_handle);
void vs_engine_destroy(VS_EngineHandle handle);

// ============================================================================
// Player (播放器) API
// ============================================================================
VS_Result vs_player_create(VS_PlayerHandle* out_handle);
void vs_player_destroy(VS_PlayerHandle handle);

// 绑定 Android Surface (通过 ANativeWindow 强转为 void*) 或 iOS UIView
VS_Result vs_player_set_window(VS_PlayerHandle handle, void* window);
VS_Result vs_player_play(VS_PlayerHandle handle);
VS_Result vs_player_pause(VS_PlayerHandle handle);
VS_Result vs_player_seek(VS_PlayerHandle handle, int64_t time_ms);

// ============================================================================
// Camera (采集) API
// ============================================================================
VS_Result vs_camera_create(VS_CameraHandle* out_handle);
void vs_camera_destroy(VS_CameraHandle handle);

// 绑定预览 Window
VS_Result vs_camera_set_preview_window(VS_CameraHandle handle, void* window);

// Kotlin 层推送 OES 纹理给 C++ 引擎
VS_Result vs_camera_push_oes_texture(VS_CameraHandle handle, uint32_t texture_id, int width, int height, int64_t timestamp_ns, const float* transform_matrix);

#ifdef __cplusplus
}
#endif
