package com.video.demo.feature.editor.ui

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import com.video.demo.feature.editor.ui.components.TimelineComponent
import com.video.demo.feature.editor.ui.components.ToolbarComponent
import com.video.demo.feature.editor.ui.components.VideoPlayerComponent

@Composable
fun EditorScreen() {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .background(Color.Black)
    ) {
        // 顶部栏：1080P 设置与导出按钮
        TopActionBar()

        // 视频预览区域
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .weight(0.4f)
                .background(Color.DarkGray)
        ) {
            VideoPlayerComponent()
        }

        // 中间控制栏：时间显示、播放/暂停、撤销
        MiddleControlBar()

        // 核心时间轴区域
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .weight(0.4f)
        ) {
            TimelineComponent()
        }

        // 底部工具栏
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .weight(0.2f)
        ) {
            ToolbarComponent()
        }
    }
}

@Composable
fun TopActionBar() {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp, vertical = 8.dp),
        horizontalArrangement = Arrangement.SpaceBetween,
        verticalAlignment = Alignment.CenterVertically
    ) {
        // 关闭按钮
        Text(text = "✖", color = Color.White)

        Row(verticalAlignment = Alignment.CenterVertically) {
            Text(text = "1080P", color = Color.White, modifier = Modifier.padding(end = 16.dp))
            Button(
                onClick = { /* TODO Export */ },
                colors = ButtonDefaults.buttonColors(containerColor = Color(0xFFFE2C55))
            ) {
                Text(text = "导出", color = Color.White)
            }
        }
    }
}

@Composable
fun MiddleControlBar() {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp, vertical = 8.dp),
        horizontalArrangement = Arrangement.SpaceBetween,
        verticalAlignment = Alignment.CenterVertically
    ) {
        Text(text = "00:00", color = Color.White)

        Row(horizontalArrangement = Arrangement.spacedBy(16.dp)) {
            Text(text = "↩", color = Color.White) // Undo
            Text(text = "▶", color = Color.White) // Play
            Text(text = "↪", color = Color.Gray)  // Redo
        }
    }
}
