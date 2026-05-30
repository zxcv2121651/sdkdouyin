package com.video.demo.feature.editor.ui.components

import android.view.SurfaceHolder
import android.view.SurfaceView
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.viewinterop.AndroidView
import com.video.sdk.VideoPlayer

@Composable
fun VideoPlayerComponent(modifier: Modifier = Modifier) {
    // 实例化底层的 C++ VideoPlayer
    val videoPlayer = remember { VideoPlayer() }

    // 页面销毁时释放底层资源
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
                        // 【最关键的一步】：Surface 创建后立即传递给 C++ 底层！
                        videoPlayer.setSurface(holder.surface)

                        // 准备并播放视频 (假设路径)
                        videoPlayer.prepare("/sdcard/test.mp4")
                        videoPlayer.play()
                    }

                    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
                        // 通知底层尺寸改变 (可选)
                    }

                    override fun surfaceDestroyed(holder: SurfaceHolder) {
                        // Surface 被销毁时，通知底层断开绑定，防止空指针或黑屏崩溃
                        videoPlayer.pause()
                        videoPlayer.setSurface(null)
                    }
                })
            }
        },
        modifier = modifier.fillMaxSize()
    )
}
