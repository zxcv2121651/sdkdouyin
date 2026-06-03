package com.video.demo.feature.editor.ui

import android.Manifest
import android.content.pm.PackageManager
import android.view.SurfaceHolder
import android.view.SurfaceView
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.viewinterop.AndroidView
import androidx.core.content.ContextCompat
import com.video.sdk.CameraCapture

@Composable
fun CameraScreen(
    onNavigateToEditor: () -> Unit,
    isDarkTheme: Boolean,
    onThemeToggle: () -> Unit
) {
    val context = LocalContext.current
    var hasPermission by remember {
        mutableStateOf(ContextCompat.checkSelfPermission(context, Manifest.permission.CAMERA) == PackageManager.PERMISSION_GRANTED)
    }

    val launcher = rememberLauncherForActivityResult(
        contract = ActivityResultContracts.RequestPermission(),
        onResult = { granted -> hasPermission = granted }
    )

    LaunchedEffect(Unit) {
        if (!hasPermission) {
            launcher.launch(Manifest.permission.CAMERA)
        }
    }

    if (hasPermission) {
        CameraPreviewContent(onNavigateToEditor, isDarkTheme, onThemeToggle)
    } else {
        Box(modifier = Modifier.fillMaxSize().background(MaterialTheme.colorScheme.background), contentAlignment = Alignment.Center) {
            Text("需要相机权限才能拍摄", color = MaterialTheme.colorScheme.onBackground)
        }
    }
}

@Composable
fun CameraPreviewContent(
    onNavigateToEditor: () -> Unit,
    isDarkTheme: Boolean,
    onThemeToggle: () -> Unit
) {
    val cameraCapture = remember { CameraCapture() }

    DisposableEffect(Unit) {
        onDispose {
            cameraCapture.stopPreview()
            cameraCapture.destroy()
        }
    }

    Box(modifier = Modifier.fillMaxSize().background(Color.Black)) {
        AndroidView(
            factory = { context ->
                SurfaceView(context).apply {
                    holder.addCallback(object : SurfaceHolder.Callback {
                        override fun surfaceCreated(holder: SurfaceHolder) {
                            cameraCapture.setPreviewSurface(holder.surface)
                            if (cameraCapture.initialize(1080, 1920, 30)) {
                                cameraCapture.startPreview()
                            }
                        }
                        override fun surfaceChanged(holder: SurfaceHolder, format: Int, w: Int, h: Int) {}
                        override fun surfaceDestroyed(holder: SurfaceHolder) {
                            cameraCapture.stopPreview()
                            cameraCapture.setPreviewSurface(null)
                        }
                    })
                }
            },
            modifier = Modifier.fillMaxSize()
        )

        // 顶部主题切换
        Row(
            modifier = Modifier.fillMaxWidth().padding(16.dp),
            horizontalArrangement = Arrangement.End
        ) {
            Text(
                text = if (isDarkTheme) "🌞" else "🌙",
                color = Color.White,
                fontSize = 24.sp,
                modifier = Modifier.clickable { onThemeToggle() }
            )
        }

        // 底部控制区
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .align(Alignment.BottomCenter)
                .padding(bottom = 50.dp),
            horizontalArrangement = Arrangement.SpaceEvenly,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Text(
                text = "🔄",
                color = Color.White,
                fontSize = 32.sp,
                modifier = Modifier.clickable { cameraCapture.switchCamera() }
            )

            Box(
                modifier = Modifier
                    .size(80.dp)
                    .clip(CircleShape)
                    .background(Color.White)
                    .clickable { onNavigateToEditor() },
                contentAlignment = Alignment.Center
            ) {
                Box(
                    modifier = Modifier
                        .size(60.dp)
                        .clip(CircleShape)
                        .background(MaterialTheme.colorScheme.primary)
                )
            }

            Text(
                text = "✓",
                color = Color.White,
                fontSize = 32.sp,
                modifier = Modifier.clickable { onNavigateToEditor() }
            )
        }
    }
}
