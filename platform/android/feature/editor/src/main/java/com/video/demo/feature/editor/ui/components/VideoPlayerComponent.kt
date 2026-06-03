package com.video.demo.feature.editor.ui.components

import android.view.SurfaceHolder
import android.view.SurfaceView
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.ui.Modifier
import androidx.compose.ui.viewinterop.AndroidView
import com.video.sdk.VideoPlayer

@Composable
fun VideoPlayerComponent(videoPlayer: VideoPlayer, modifier: Modifier = Modifier) {
    DisposableEffect(Unit) {
        onDispose {
            videoPlayer.destroy()
        }
    }

    AndroidView(
        factory = { context ->
            SurfaceView(context).apply {
                holder.addCallback(object : SurfaceHolder.Callback {
                    override fun surfaceCreated(holder: SurfaceHolder) {
                        videoPlayer.setSurface(holder.surface)
                        videoPlayer.prepare("/sdcard/test.mp4")
                    }

                    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {}

                    override fun surfaceDestroyed(holder: SurfaceHolder) {
                        videoPlayer.pause()
                        videoPlayer.setSurface(null)
                    }
                })
            }
        },
        modifier = modifier.fillMaxSize()
    )
}
