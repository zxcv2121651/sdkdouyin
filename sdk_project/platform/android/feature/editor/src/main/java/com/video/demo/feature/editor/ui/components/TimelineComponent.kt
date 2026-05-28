package com.video.demo.feature.editor.ui.components

import androidx.compose.foundation.background
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp

data class TrackInfo(val trackId: String, val clips: List<ClipUiModel>)
data class ClipUiModel(val id: String, val durationMs: Long, val color: Color)

@Composable
fun TimelineComponent(
    tracks: List<TrackInfo>,
    currentTimeMs: Long,
    onSeek: (Long) -> Unit,
    modifier: Modifier = Modifier
) {
    Box(
        modifier = modifier
            .fillMaxWidth()
            .height(200.dp)
            .background(MaterialTheme.colorScheme.surface)
    ) {
        val scrollState = rememberScrollState()

        // 轨道区域 (支持横向滑动)
        Column(
            modifier = Modifier
                .fillMaxSize()
                .horizontalScroll(scrollState)
                .padding(vertical = 16.dp, horizontal = 50.dp) // 预留吸附中线的空间
        ) {
            tracks.forEach { track ->
                TrackRow(track)
                Spacer(modifier = Modifier.height(8.dp))
            }
        }

        // 吸附中线 (播放指针)
        Box(
            modifier = Modifier
                .align(Alignment.Center)
                .width(2.dp)
                .fillMaxHeight()
                .background(Color.White)
        )
    }
}

@Composable
private fun TrackRow(track: TrackInfo) {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .height(50.dp),
        verticalAlignment = Alignment.CenterVertically
    ) {
        track.clips.forEach { clip ->
            // 这里将 durationMs 转换为屏幕 dp 宽度 (比如 1ms = 0.01dp)
            val widthDp = (clip.durationMs * 0.01f).dp
            Box(
                modifier = Modifier
                    .width(widthDp)
                    .fillMaxHeight()
                    .padding(end = 2.dp)
                    .background(clip.color, RoundedCornerShape(4.dp))
            ) {
                // 内部可以渲染缩略图
            }
        }
    }
}
