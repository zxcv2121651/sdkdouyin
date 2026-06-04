package com.video.demo.feature.editor.ui

import android.Manifest
import android.content.pm.PackageManager
import android.graphics.SurfaceTexture
import android.hardware.camera2.CameraManager
import android.opengl.GLES11Ext
import android.opengl.GLES20
import android.view.TextureView
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
    val context = LocalContext.current
    val cameraCapture = remember { CameraCapture() }

    // In a real application, CameraX or Camera2 would manage this logic.
    // Here we simulate the Kotlin-side OES texture management.
    var previewTextureId by remember { mutableStateOf(0) }
    var surfaceTexture by remember { mutableStateOf<SurfaceTexture?>(null) }
    val transformMatrix = FloatArray(16)

    DisposableEffect(Unit) {
        onDispose {
            cameraCapture.destroy()
            surfaceTexture?.release()
        }
    }

    Box(modifier = Modifier.fillMaxSize().background(Color.Black)) {
        AndroidView(
            factory = { ctx ->
                TextureView(ctx).apply {
                    surfaceTextureListener = object : TextureView.SurfaceTextureListener {
                        override fun onSurfaceTextureAvailable(st: SurfaceTexture, width: Int, height: Int) {
                            // 1. Generate OES Texture ID
                            val textures = IntArray(1)
                            GLES20.glGenTextures(1, textures, 0)
                            previewTextureId = textures[0]
                            GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, previewTextureId)

                            // 2. Attach SurfaceTexture to this OpenGL Texture
                            // Note: In real app, we use a separate rendering thread with EGL Context.
                            // Here we just instantiate to show the architecture.
                            val internalSurfaceTexture = SurfaceTexture(previewTextureId)
                            internalSurfaceTexture.setDefaultBufferSize(width, height)
                            surfaceTexture = internalSurfaceTexture

                            internalSurfaceTexture.setOnFrameAvailableListener { st ->
                                // 3. Update texture and push to C++ Engine
                                st.updateTexImage()
                                st.getTransformMatrix(transformMatrix)
                                cameraCapture.pushOESTexture(previewTextureId, width, height, st.timestamp, transformMatrix)
                            }

                            // 4. Open Camera and start preview with internalSurfaceTexture ...
                            // (CameraX or Camera2 API binding omitted for brevity)
                        }

                        override fun onSurfaceTextureSizeChanged(st: SurfaceTexture, width: Int, height: Int) {}
                        override fun onSurfaceTextureDestroyed(st: SurfaceTexture): Boolean {
                            return true
                        }
                        override fun onSurfaceTextureUpdated(st: SurfaceTexture) {}
                    }
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
                modifier = Modifier.clickable { /* switch camera */ }
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
