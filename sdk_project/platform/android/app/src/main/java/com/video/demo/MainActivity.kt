package com.video.demo

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.isSystemInDarkTheme
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import com.video.demo.core.designsystem.theme.VideoEditorTheme
import com.video.demo.feature.editor.ui.CameraScreen
import com.video.demo.feature.editor.ui.EditorScreen

/**
 * 屏幕路由枚举
 */
enum class Screen {
    CAMERA, // 摄像采集页
    EDITOR  // 剪辑工作台页
}

/**
 * 演示 App 的唯一主 Activity。
 * 本项目遵循 Single Activity 架构，UI 完全由 Jetpack Compose 构建和驱动。
 */
class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        setContent {
            // 全局主题状态提升：控制全 App 处于深色模式还是浅色模式
            // 默认跟随系统主题，但允许用户通过 UI 按钮手动切换 (isDarkTheme 变量)
            var isDarkTheme by remember { mutableStateOf(true) }

            // 包裹全局的 Material 3 自定义主题 (定义在 core/designsystem 模块中)
            VideoEditorTheme(darkTheme = isDarkTheme) {
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colorScheme.background
                ) {
                    // Compose 原生导航状态：记录当前处于哪个界面
                    var currentScreen by remember { mutableStateOf(Screen.CAMERA) }

                    // 根据状态决定渲染哪个全屏的 Composable 函数
                    when (currentScreen) {
                        Screen.CAMERA -> {
                            CameraScreen(
                                onNavigateToEditor = { currentScreen = Screen.EDITOR },
                                isDarkTheme = isDarkTheme,
                                onThemeToggle = { isDarkTheme = !isDarkTheme }
                            )
                        }
                        Screen.EDITOR -> {
                            EditorScreen(
                                onBack = { currentScreen = Screen.CAMERA },
                                isDarkTheme = isDarkTheme,
                                onThemeToggle = { isDarkTheme = !isDarkTheme }
                            )
                        }
                    }
                }
            }
        }
    }
}
