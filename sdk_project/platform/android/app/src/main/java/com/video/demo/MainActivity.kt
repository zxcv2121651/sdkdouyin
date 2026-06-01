package com.video.demo

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import com.video.demo.feature.editor.ui.CameraScreen
import com.video.demo.feature.editor.ui.EditorScreen

enum class Screen {
    CAMERA,
    EDITOR
}

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            MaterialTheme {
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colorScheme.background
                ) {
                    var currentScreen by remember { mutableStateOf(Screen.CAMERA) }

                    when (currentScreen) {
                        Screen.CAMERA -> {
                            CameraScreen(
                                onNavigateToEditor = { currentScreen = Screen.EDITOR }
                            )
                        }
                        Screen.EDITOR -> {
                            EditorScreen(
                                onBack = { currentScreen = Screen.CAMERA }
                            )
                        }
                    }
                }
            }
        }
    }
}
