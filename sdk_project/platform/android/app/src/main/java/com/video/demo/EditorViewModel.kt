package com.video.demo

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.video.sdk.VideoEditor
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.launch
import kotlinx.coroutines.delay

/**
 * UI 层的数据状态定义
 */
data class EditorUiState(
    val isPlaying: Boolean = false,
    val currentPositionMs: Long = 0L,
    val durationMs: Long = 0L,
    val clips: List<ClipInfo> = emptyList()
)

data class ClipInfo(val id: String, val thumbnailPath: String, val durationMs: Long)

/**
 * 剪辑核心 ViewModel，桥接 Jetpack Compose UI 和底层 C++ SDK。
 * 对标剪映：保证主线程无阻塞，所有耗时调用（如导入、导出、底层状态轮询）都在协程中异步处理。
 */
class EditorViewModel : ViewModel() {

    private val _uiState = MutableStateFlow(EditorUiState())
    val uiState: StateFlow<EditorUiState> = _uiState.asStateFlow()

    // 真正的底层 C++ 引擎门面
    private var videoEditor: VideoEditor? = null

    init {
        // 初始化 SDK
        videoEditor = VideoEditor()

        // 模拟开始一个循环轮询底层的播放进度，驱动时间轴 UI 刷新
        startProgressPolling()
    }

    /**
     * 导入用户从相册选择的视频
     */
    fun importVideo(filePath: String) {
        viewModelScope.launch {
            try {
                // 模拟获取一个视频的时长
                val mockDuration = 10000L

                // 工业级做法：通常 SDK 会有异步回调，或者挂起函数来完成耗时的首帧解析
                videoEditor?.importVideo(filePath, 0L)

                // 更新 UI 状态，新增一个片段
                val currentClips = _uiState.value.clips.toMutableList()
                currentClips.add(ClipInfo(id = "clip_${System.currentTimeMillis()}", thumbnailPath = filePath, durationMs = mockDuration))

                val newDuration = currentClips.sumOf { it.durationMs }
                _uiState.value = _uiState.value.copy(clips = currentClips, durationMs = newDuration)

            } catch (e: Exception) {
                // 异常统一捕获处理，底层的 VideoSdkError 会被抛到这里
                e.printStackTrace()
            }
        }
    }

    fun togglePlayPause() {
        val playing = !_uiState.value.isPlaying
        _uiState.value = _uiState.value.copy(isPlaying = playing)
        if (playing) {
            // videoEditor?.play()
        } else {
            // videoEditor?.pause()
        }
    }

    fun seekTo(positionMs: Long) {
        _uiState.value = _uiState.value.copy(currentPositionMs = positionMs)
        // videoEditor?.seekTo(positionMs)
    }

    private fun startProgressPolling() {
        viewModelScope.launch {
            while (true) {
                if (_uiState.value.isPlaying) {
                    // 模拟进度推进。实际应该从底层 Player 获取真正的音视频同步时钟
                    val newPos = _uiState.value.currentPositionMs + 33L // 约 30fps
                    if (newPos >= _uiState.value.durationMs) {
                        _uiState.value = _uiState.value.copy(isPlaying = false, currentPositionMs = _uiState.value.durationMs)
                    } else {
                        _uiState.value = _uiState.value.copy(currentPositionMs = newPos)
                    }
                }
                delay(33L)
            }
        }
    }

    override fun onCleared() {
        super.onCleared()
        videoEditor?.destroy()
    }
}
