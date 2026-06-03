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
fun EditorScreen(
    onBack: () -> Unit,
    isDarkTheme: Boolean,
    onThemeToggle: () -> Unit
) {
    val videoPlayer = remember { VideoPlayer() }
    val exporter = remember { TimelineExporter() }

    var isPlaying by remember { mutableStateOf(false) }
    var isExporting by remember { mutableStateOf(false) }
    var exportProgress by remember { mutableStateOf(0f) }
    val coroutineScope = rememberCoroutineScope()

    Box(modifier = Modifier.fillMaxSize().background(MaterialTheme.colorScheme.background)) {
        Column(modifier = Modifier.fillMaxSize()) {
            // 顶部栏
            TopActionBar(
                onClose = onBack,
                isDarkTheme = isDarkTheme,
                onThemeToggle = onThemeToggle,
                onExport = {
                    isExporting = true
                    exporter.startExport("/sdcard/Download/output.mp4")

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

            // 视频预览区域 (预览区始终保持较深的颜色比较好)
            Box(modifier = Modifier.fillMaxWidth().weight(0.4f).background(Color(0xFF222222))) {
                VideoPlayerComponent(videoPlayer = videoPlayer)
            }

            // 中间控制栏
            MiddleControlBar(
                isPlaying = isPlaying,
                onPlayPauseToggle = {
                    if (isPlaying) videoPlayer.pause() else videoPlayer.play()
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
fun TopActionBar(
    onClose: () -> Unit,
    isDarkTheme: Boolean,
    onThemeToggle: () -> Unit,
    onExport: () -> Unit
) {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .background(MaterialTheme.colorScheme.background)
            .padding(horizontal = 16.dp, vertical = 8.dp),
        horizontalArrangement = Arrangement.SpaceBetween,
        verticalAlignment = Alignment.CenterVertically
    ) {
        Text(text = "✖", color = MaterialTheme.colorScheme.onBackground, modifier = Modifier.clickable { onClose() }.padding(8.dp))

        Row(verticalAlignment = Alignment.CenterVertically) {
            // 主题切换按钮
            Text(
                text = if (isDarkTheme) "🌞" else "🌙",
                modifier = Modifier.clickable { onThemeToggle() }.padding(end = 24.dp),
                color = MaterialTheme.colorScheme.onBackground
            )

            Text(text = "1080P", color = MaterialTheme.colorScheme.onBackground, modifier = Modifier.padding(end = 16.dp))
            Button(
                onClick = onExport,
                colors = ButtonDefaults.buttonColors(containerColor = MaterialTheme.colorScheme.primary),
                contentPadding = PaddingValues(horizontal = 16.dp, vertical = 4.dp)
            ) {
                Text(text = "导出", color = Color.White) // 导出文字始终是白色
            }
        }
    }
}

@Composable
fun MiddleControlBar(isPlaying: Boolean, onPlayPauseToggle: () -> Unit) {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .background(MaterialTheme.colorScheme.background)
            .padding(horizontal = 16.dp, vertical = 8.dp),
        horizontalArrangement = Arrangement.SpaceBetween,
        verticalAlignment = Alignment.CenterVertically
    ) {
        Text(text = "00:00", color = MaterialTheme.colorScheme.onBackground)

        Row(horizontalArrangement = Arrangement.spacedBy(24.dp), verticalAlignment = Alignment.CenterVertically) {
            Text(text = "↩", color = MaterialTheme.colorScheme.onBackground)
            Text(
                text = if (isPlaying) "⏸" else "▶",
                color = MaterialTheme.colorScheme.onBackground,
                style = MaterialTheme.typography.titleLarge,
                modifier = Modifier.clickable { onPlayPauseToggle() }
            )
            Text(text = "↪", color = MaterialTheme.colorScheme.onSurfaceVariant)
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
                    color = MaterialTheme.colorScheme.primary,
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
