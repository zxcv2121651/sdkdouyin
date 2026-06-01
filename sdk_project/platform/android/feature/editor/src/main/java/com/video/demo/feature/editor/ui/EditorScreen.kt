package com.video.demo.feature.editor.ui

import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import androidx.compose.ui.window.Dialog
import com.video.demo.feature.editor.ui.components.TimelineComponent
import com.video.demo.feature.editor.ui.components.ToolbarComponent
import com.video.demo.feature.editor.ui.components.VideoPlayerComponent
import com.video.sdk.VideoPlayer
import com.video.sdk.TimelineExporter
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch

@Composable
fun EditorScreen(onBack: () -> Unit) {
    val videoPlayer = remember { VideoPlayer() }
    val exporter = remember { TimelineExporter() }

    var isPlaying by remember { mutableStateOf(false) }
    var isExporting by remember { mutableStateOf(false) }
    var exportProgress by remember { mutableStateOf(0f) }
    val coroutineScope = rememberCoroutineScope()

    Box(modifier = Modifier.fillMaxSize().background(Color.Black)) {
        Column(modifier = Modifier.fillMaxSize()) {
            // 顶部栏
            TopActionBar(
                onClose = onBack,
                onExport = {
                    isExporting = true
                    exporter.startExport("/sdcard/Download/output.mp4")

                    // 轮询进度
                    coroutineScope.launch {
                        while (isExporting) {
                            exportProgress = exporter.getProgress()
                            if (exportProgress >= 1.0f) {
                                isExporting = false
                                break
                            }
                            delay(100)
                        }
                    }
                }
            )

            // 视频预览区域
            Box(modifier = Modifier.fillMaxWidth().weight(0.4f).background(Color.DarkGray)) {
                VideoPlayerComponent(videoPlayer = videoPlayer)
            }

            // 中间控制栏
            MiddleControlBar(
                isPlaying = isPlaying,
                onPlayPauseToggle = {
                    if (isPlaying) {
                        videoPlayer.pause()
                    } else {
                        videoPlayer.play()
                    }
                    isPlaying = !isPlaying
                }
            )

            // 时间轴区域
            Box(modifier = Modifier.fillMaxWidth().weight(0.4f)) {
                TimelineComponent()
            }

            // 底部工具栏
            Box(modifier = Modifier.fillMaxWidth().weight(0.2f)) {
                ToolbarComponent()
            }
        }

        // 导出进度遮罩层
        if (isExporting) {
            ExportingOverlay(
                progress = exportProgress,
                onCancel = {
                    exporter.cancelExport()
                    isExporting = false
                }
            )
        }
    }
}

@Composable
fun TopActionBar(onClose: () -> Unit, onExport: () -> Unit) {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp, vertical = 8.dp),
        horizontalArrangement = Arrangement.SpaceBetween,
        verticalAlignment = Alignment.CenterVertically
    ) {
        Text(text = "✖", color = Color.White, modifier = Modifier.clickable { onClose() }.padding(8.dp))

        Row(verticalAlignment = Alignment.CenterVertically) {
            Text(text = "1080P", color = Color.White, modifier = Modifier.padding(end = 16.dp))
            Button(
                onClick = onExport,
                colors = ButtonDefaults.buttonColors(containerColor = Color(0xFFFE2C55)),
                contentPadding = PaddingValues(horizontal = 16.dp, vertical = 4.dp)
            ) {
                Text(text = "导出", color = Color.White)
            }
        }
    }
}

@Composable
fun MiddleControlBar(isPlaying: Boolean, onPlayPauseToggle: () -> Unit) {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp, vertical = 8.dp),
        horizontalArrangement = Arrangement.SpaceBetween,
        verticalAlignment = Alignment.CenterVertically
    ) {
        Text(text = "00:00", color = Color.White)

        Row(horizontalArrangement = Arrangement.spacedBy(24.dp), verticalAlignment = Alignment.CenterVertically) {
            Text(text = "↩", color = Color.White) // Undo
            Text(
                text = if (isPlaying) "⏸" else "▶",
                color = Color.White,
                style = MaterialTheme.typography.titleLarge,
                modifier = Modifier.clickable { onPlayPauseToggle() }
            )
            Text(text = "↪", color = Color.Gray)  // Redo
        }
    }
}

@Composable
fun ExportingOverlay(progress: Float, onCancel: () -> Unit) {
    Dialog(onDismissRequest = { /* 阻断外部点击 */ }) {
        Box(
            modifier = Modifier
                .size(200.dp)
                .background(Color(0xDD000000), shape = MaterialTheme.shapes.medium),
            contentAlignment = Alignment.Center
        ) {
            Column(horizontalAlignment = Alignment.CenterHorizontally) {
                CircularProgressIndicator(
                    progress = progress,
                    color = Color(0xFFFE2C55),
                    modifier = Modifier.size(60.dp)
                )
                Spacer(modifier = Modifier.height(16.dp))
                Text(text = "导出中... ${(progress * 100).toInt()}%", color = Color.White)
                Spacer(modifier = Modifier.height(16.dp))
                Text(
                    text = "取消",
                    color = Color.Gray,
                    modifier = Modifier.clickable { onCancel() }.padding(8.dp)
                )
            }
        }
    }
}
