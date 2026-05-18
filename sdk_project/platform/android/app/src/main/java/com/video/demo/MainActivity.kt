package com.video.demo

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.viewModels
import androidx.compose.material3.MaterialTheme
import androidx.core.view.WindowCompat
import com.video.demo.ui.EditorScreen

class MainActivity : ComponentActivity() {

    // 工业级应用推荐使用 ViewModel 来保持 UI 与逻辑的分离
    private val viewModel: EditorViewModel by viewModels()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // 剪映规范：沉浸式状态栏与全屏
        WindowCompat.setDecorFitsSystemWindows(window, false)

        setContent {
            MaterialTheme {
                EditorScreen(viewModel = viewModel)
            }
        }
    }
}
