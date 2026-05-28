package com.video.demo.core.designsystem.theme

import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.darkColorScheme
import androidx.compose.runtime.Composable
import androidx.compose.ui.graphics.Color

val JianYingDarkColors = darkColorScheme(
    background = Color(0xFF111111),
    surface = Color(0xFF1C1C1C),
    primary = Color(0xFFFE2C55), // 抖音红/剪映红
    onPrimary = Color.White,
    onBackground = Color.White,
    onSurface = Color.LightGray
)

@Composable
fun VideoEditorTheme(content: @Composable () -> Unit) {
    MaterialTheme(
        colorScheme = JianYingDarkColors,
        content = content
    )
}
