package com.video.demo.feature.editor.domain

import com.video.sdk.VideoEditor
// import javax.inject.Inject
// import javax.inject.Singleton

/**
 * 领域层接口，抽象底层的 SDK 实现
 */
interface IEditorRepository {
    suspend fun importVideo(path: String, startMs: Long): Result<Unit>
    fun seekTo(timeMs: Long)
    fun destroy()
}

/**
 * 实际的 Repository 实现，通过 DI 注入
 */
// @Singleton
class EditorRepositoryImpl /* @Inject constructor */() : IEditorRepository {

    private val sdkEngine = VideoEditor()

    override suspend fun importVideo(path: String, startMs: Long): Result<Unit> {
        return try {
            sdkEngine.importVideo(path, startMs)
            Result.success(Unit)
        } catch (e: Exception) {
            Result.failure(e)
        }
    }

    override fun seekTo(timeMs: Long) {
        // sdkEngine.seekTo(timeMs)
    }

    override fun destroy() {
        sdkEngine.destroy()
    }
}
