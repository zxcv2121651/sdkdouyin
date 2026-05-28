package com.video.demo

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.core.view.WindowCompat
// import dagger.hilt.android.AndroidEntryPoint
import com.video.demo.feature.editor.ui.EditorScreenRouter

// @AndroidEntryPoint
class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        WindowCompat.setDecorFitsSystemWindows(window, false)
        setContent {
            // App 壳工程直接路由到 Editor 模块的根节点
            EditorScreenRouter()
        }
    }
}
