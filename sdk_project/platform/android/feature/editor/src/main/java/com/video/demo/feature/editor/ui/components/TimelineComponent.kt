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
import androidx.compose.ui.draw.clip
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

@Composable
fun TimelineComponent() {
    Box(
        modifier = Modifier
            .fillMaxSize()
            .background(MaterialTheme.colorScheme.surface) // 轨道大背景
    ) {
        val scrollState = rememberScrollState()

        Row(
            modifier = Modifier
                .fillMaxSize()
                .horizontalScroll(scrollState)
                .padding(top = 16.dp, bottom = 16.dp)
        ) {
            Spacer(modifier = Modifier.width(180.dp))

            Column {
                TimeRuler()
                Spacer(modifier = Modifier.height(8.dp))

                Row(
                    modifier = Modifier
                        .height(60.dp)
                        .clip(RoundedCornerShape(8.dp))
                        .background(MaterialTheme.colorScheme.surfaceVariant), // 轨道底色
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Box(
                        modifier = Modifier
                            .fillMaxHeight()
                            .width(300.dp)
                            .background(MaterialTheme.colorScheme.onSurfaceVariant.copy(alpha = 0.5f)) // Clip颜色
                    ) {
                        Text(
                            text = "视频片段 1",
                            color = MaterialTheme.colorScheme.onSurface,
                            modifier = Modifier.align(Alignment.Center)
                        )
                    }

                    Spacer(modifier = Modifier.width(2.dp))

                    Box(
                        modifier = Modifier
                            .fillMaxHeight()
                            .width(200.dp)
                            .background(MaterialTheme.colorScheme.onSurfaceVariant.copy(alpha = 0.5f))
                    ) {
                        Text(
                            text = "视频片段 2",
                            color = MaterialTheme.colorScheme.onSurface,
                            modifier = Modifier.align(Alignment.Center)
                        )
                    }
                }
            }

            Spacer(modifier = Modifier.width(180.dp))
        }

        // 播放准心线，固定在中间
        Box(
            modifier = Modifier
                .align(Alignment.Center)
                .width(2.dp)
                .fillMaxHeight()
                .padding(vertical = 4.dp)
                .background(MaterialTheme.colorScheme.onBackground)
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
                Text(text = "00:0$i", color = MaterialTheme.colorScheme.onSurfaceVariant, fontSize = 10.sp)
                Box(modifier = Modifier.height(4.dp).width(1.dp).background(MaterialTheme.colorScheme.onSurfaceVariant))
            }
        }
    }
}
