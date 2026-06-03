package com.video.demo.feature.editor.ui

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import com.video.demo.core.designsystem.theme.VideoEditorTheme
import com.video.demo.feature.editor.ui.components.*

@Composable
fun EditorScreenRouter() {
    VideoEditorTheme {
        Column(modifier = Modifier.fillMaxSize()) {

            // 1. 顶部栏 (省略封装)

            // 2. 独立组件：播放器
            VideoPlayerComponent(
                modifier = Modifier.weight(1f),
                onSurfaceReady = { surface -> /* 通知 SDK 绑定 EGL */ },
                onSurfaceDestroyed = { /* 解除 EGL */ }
            )

            // 3. 独立组件：时间轴
            TimelineComponent(
                tracks = listOf(
                    TrackInfo("v1", listOf(
                        ClipUiModel("c1", 5000L, Color.Gray),
                        ClipUiModel("c2", 8000L, Color.DarkGray)
                    ))
                ),
                currentTimeMs = 0L,
                onSeek = { }
            )

            // 4. 独立组件：工具栏
            ToolbarComponent(
                items = listOf(
                    ToolbarItem("剪辑") {},
                    ToolbarItem("音频") {},
                    ToolbarItem("文字") {},
                    ToolbarItem("画中画") {},
                    ToolbarItem("特效") {}
                )
            )
        }
    }
}
