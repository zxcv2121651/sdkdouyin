package com.video.demo.feature.editor.ui.components

import androidx.compose.foundation.background
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

@Composable
fun TimelineComponent() {
    Box(
        modifier = Modifier
            .fillMaxSize()
            .background(Color(0xFF1E1E1E))
    ) {
        val scrollState = rememberScrollState()

        // 刻度尺和视频轨道包裹在一个横向滚动的 Row 中
        Row(
            modifier = Modifier
                .fillMaxSize()
                .horizontalScroll(scrollState)
                .padding(top = 16.dp, bottom = 16.dp)
        ) {
            // 左侧占位（保证中心线在起播位置）
            Spacer(modifier = Modifier.width(180.dp))

            Column {
                // 顶部刻度尺模拟
                TimeRuler()
                Spacer(modifier = Modifier.height(8.dp))

                // 视频轨道
                Row(
                    modifier = Modifier
                        .height(60.dp)
                        .clip(RoundedCornerShape(8.dp))
                        .background(Color(0xFF333333)),
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    // 模拟一段视频 Clip
                    Box(
                        modifier = Modifier
                            .fillMaxHeight()
                            .width(300.dp)
                            .background(Color(0xFF4A4A4A))
                    ) {
                        Text(
                            text = "视频片段 1",
                            color = Color.White,
                            modifier = Modifier.align(Alignment.Center)
                        )
                    }

                    Spacer(modifier = Modifier.width(2.dp)) // 视频段间距

                    // 模拟第二段视频 Clip
                    Box(
                        modifier = Modifier
                            .fillMaxHeight()
                            .width(200.dp)
                            .background(Color(0xFF4A4A4A))
                    ) {
                        Text(
                            text = "视频片段 2",
                            color = Color.White,
                            modifier = Modifier.align(Alignment.Center)
                        )
                    }
                }
            }

            // 右侧占位
            Spacer(modifier = Modifier.width(180.dp))
        }

        // 固定的白色中心播放线 (Playhead)
        Box(
            modifier = Modifier
                .align(Alignment.Center)
                .width(2.dp)
                .fillMaxHeight()
                .padding(vertical = 4.dp)
                .background(Color.White)
        )
    }
}

@Composable
fun TimeRuler() {
    Row(modifier = Modifier.width(500.dp)) {
        for (i in 0..10) {
            Column(
                horizontalAlignment = Alignment.CenterHorizontally,
                modifier = Modifier.width(50.dp)
            ) {
                Text(text = "00:0$i", color = Color.Gray, fontSize = 10.sp)
                Box(modifier = Modifier.height(4.dp).width(1.dp).background(Color.Gray))
            }
        }
    }
}
