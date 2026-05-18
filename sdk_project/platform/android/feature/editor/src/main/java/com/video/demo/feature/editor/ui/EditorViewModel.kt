package com.video.demo.feature.editor.ui

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.video.demo.feature.editor.domain.IEditorRepository
// import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.launch
// import javax.inject.Inject

// @HiltViewModel
class EditorViewModel /* @Inject constructor */(
    private val repository: IEditorRepository
) : ViewModel() {

    fun userActionImport(path: String) {
        viewModelScope.launch {
            repository.importVideo(path, 0L)
                .onSuccess {
                    // Update StateFlow
                }
                .onFailure { error ->
                    // Show error toast
                }
        }
    }

    override fun onCleared() {
        super.onCleared()
        repository.destroy()
    }
}
