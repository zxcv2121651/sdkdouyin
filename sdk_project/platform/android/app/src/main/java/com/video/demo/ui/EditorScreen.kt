package com.video.demo.ui

import android.view.SurfaceView
import androidx.compose.foundation.background
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.viewinterop.AndroidView
import com.video.demo.EditorUiState
import com.video.demo.EditorViewModel

@Composable
fun EditorScreen(viewModel: EditorViewModel) {
    val uiState by viewModel.uiState.collectAsState()

    Column(
        modifier = Modifier
            .fillMaxSize()
            .background(Color(0xFF121212)) // 剪映经典的深色工作台风格
    ) {
        // 1. 顶部导航栏 (Top Bar)
        TopNavBar()

        // 2. 视频预览区 (Preview Area - EGL/SurfaceView 绑定处)
        VideoPreviewArea(modifier = Modifier.weight(1f))

        // 3. 播放控制与时间信息 (Play Controls)
        PlayControls(
            isPlaying = uiState.isPlaying,
            currentPos = uiState.currentPositionMs,
            duration = uiState.durationMs,
            onTogglePlay = { viewModel.togglePlayPause() }
        )

        // 4. 多轨时间轴区 (Timeline Area)
        TimelineArea(
            uiState = uiState,
            onSeek = { viewModel.seekTo(it) }
        )

        // 5. 底部工具栏 (Bottom Toolbar - 一级菜单)
        BottomToolbar(
            onImportClick = {
                // 模拟传入一个视频路径
                viewModel.importVideo("/sdcard/DCIM/test_video.mp4")
            }
        )
    }
}

@Composable
fun TopNavBar() {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .height(56.dp)
            .padding(horizontal = 16.dp),
        horizontalArrangement = Arrangement.SpaceBetween,
        verticalAlignment = Alignment.CenterVertically
    ) {
        Text("×", color = Color.White, fontSize = 28.sp)
        Row(verticalAlignment = Alignment.CenterVertically) {
            Text("1080P", color = Color.White, fontSize = 14.sp, modifier = Modifier.padding(end = 16.dp))
            Button(
                onClick = { /* 触发 TimelineExporter */ },
                colors = ButtonDefaults.buttonColors(containerColor = Color(0xFFFE2C55)),
                shape = RoundedCornerShape(4.dp),
                contentPadding = PaddingValues(horizontal = 16.dp, vertical = 4.dp)
            ) {
                Text("导出", color = Color.White, fontWeight = FontWeight.Bold)
            }
        }
    }
}

@Composable
fun VideoPreviewArea(modifier: Modifier = Modifier) {
    Box(
        modifier = modifier
            .fillMaxWidth()
            .background(Color.Black),
        contentAlignment = Alignment.Center
    ) {
        // 核心：使用 SurfaceView 提供 Native Window 给底层的 EGLCore 渲染
        AndroidView(
            factory = { context ->
                SurfaceView(context).apply {
                    holder.addCallback(object : android.view.SurfaceHolder.Callback {
                        override fun surfaceCreated(holder: android.view.SurfaceHolder) {
                            // 通知到底层 EGLCore::createWindowSurface(holder.surface)
                        }
                        override fun surfaceChanged(holder: android.view.SurfaceHolder, format: Int, w: Int, h: Int) {}
                        override fun surfaceDestroyed(holder: android.view.SurfaceHolder) {
                            // 通知底层销毁 EGLSurface
                        }
                    })
                }
            },
            modifier = Modifier.aspectRatio(9f/16f) // 竖屏短视频比例
        )
    }
}

@Composable
fun PlayControls(isPlaying: Boolean, currentPos: Long, duration: Long, onTogglePlay: () -> Unit) {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(vertical = 8.dp, horizontal = 16.dp),
        horizontalArrangement = Arrangement.SpaceBetween,
        verticalAlignment = Alignment.CenterVertically
    ) {
        val formatTime = { ms: Long -> String.format("%02d:%02d", (ms / 1000) / 60, (ms / 1000) % 60) }
        Text(formatTime(currentPos), color = Color.White, fontSize = 12.sp)

        IconButton(onClick = onTogglePlay) {
            Text(if (isPlaying) "⏸" else "▶", color = Color.White, fontSize = 24.sp)
        }

        Text(formatTime(duration), color = Color.Gray, fontSize = 12.sp)
    }
}

@Composable
fun TimelineArea(uiState: EditorUiState, onSeek: (Long) -> Unit) {
    Box(
        modifier = Modifier
            .fillMaxWidth()
            .height(150.dp)
            .background(Color(0xFF1E1E1E))
    ) {
        // 时间轴轨道
        Row(
            modifier = Modifier
                .fillMaxSize()
                .horizontalScroll(rememberScrollState())
                .padding(vertical = 20.dp, horizontal = 16.dp) // padding 模拟留白
        ) {
            uiState.clips.forEach { clip ->
                Box(
                    modifier = Modifier
                        .height(60.dp)
                        .width(100.dp) // 模拟宽度，实际需根据 duration 计算
                        .padding(end = 2.dp)
                        .background(Color(0xFF333333), RoundedCornerShape(4.dp)),
                    contentAlignment = Alignment.Center
                ) {
                    Text("Clip", color = Color.White, fontSize = 10.sp)
                }
            }
        }

        // 播放指针 (白色中线)
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
fun BottomToolbar(onImportClick: () -> Unit) {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .height(60.dp)
            .background(Color(0xFF121212)),
        horizontalArrangement = Arrangement.SpaceEvenly,
        verticalAlignment = Alignment.CenterVertically
    ) {
        ToolBarItem("剪辑", onClick = onImportClick)
        ToolBarItem("音频", onClick = {})
        ToolBarItem("文字", onClick = {})
        ToolBarItem("特效", onClick = {})
        ToolBarItem("滤镜", onClick = {})
    }
}

@Composable
fun ToolBarItem(title: String, onClick: () -> Unit) {
    TextButton(onClick = onClick) {
        Text(title, color = Color.LightGray, fontSize = 12.sp)
    }
}
