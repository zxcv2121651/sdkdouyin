package com.video.demo.feature.editor.ui.components

import android.view.SurfaceView
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.aspectRatio
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.viewinterop.AndroidView

@Composable
fun VideoPlayerComponent(
    modifier: Modifier = Modifier,
    onSurfaceReady: (android.view.Surface) -> Unit,
    onSurfaceDestroyed: () -> Unit
) {
    Box(
        modifier = modifier
            .fillMaxWidth()
            .background(Color.Black),
        contentAlignment = Alignment.Center
    ) {
        AndroidView(
            factory = { context ->
                SurfaceView(context).apply {
                    holder.addCallback(object : android.view.SurfaceHolder.Callback {
                        override fun surfaceCreated(holder: android.view.SurfaceHolder) {
                            onSurfaceReady(holder.surface)
                        }
                        override fun surfaceChanged(holder: android.view.SurfaceHolder, format: Int, width: Int, height: Int) {}
                        override fun surfaceDestroyed(holder: android.view.SurfaceHolder) {
                            onSurfaceDestroyed()
                        }
                    })
                }
            },
            modifier = Modifier.aspectRatio(9f / 16f) // 适配短视频比例
        )
    }
}
