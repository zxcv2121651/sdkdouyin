package com.video.demo.feature.editor.ui.components

import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.material3.Icon
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

@Composable
fun ToolbarComponent() {
    Row(
        modifier = Modifier
            .fillMaxSize()
            .background(Color.Black)
            .horizontalScroll(rememberScrollState())
            .padding(horizontal = 16.dp),
        verticalAlignment = Alignment.CenterVertically,
        horizontalArrangement = Arrangement.spacedBy(24.dp)
    ) {
        ToolButton(text = "剪辑", icon = "✂")
        ToolButton(text = "音频", icon = "🎵")
        ToolButton(text = "文本", icon = "T")
        ToolButton(text = "贴纸", icon = "★")
        ToolButton(text = "画中画", icon = "◫")
        ToolButton(text = "特效", icon = "✨")
        ToolButton(text = "滤镜", icon = "◑")
        ToolButton(text = "比例", icon = "▤")
        ToolButton(text = "背景", icon = "□")
    }
}

@Composable
fun ToolButton(text: String, icon: String, onClick: () -> Unit = {}) {
    Column(
        horizontalAlignment = Alignment.CenterHorizontally,
        modifier = Modifier.clickable { onClick() }
    ) {
        // 使用 Text 暂时代替 Icon 图片
        Text(
            text = icon,
            color = Color.White,
            fontSize = 20.sp,
            modifier = Modifier.padding(bottom = 4.dp)
        )
        Text(
            text = text,
            color = Color.White,
            fontSize = 12.sp
        )
    }
}
