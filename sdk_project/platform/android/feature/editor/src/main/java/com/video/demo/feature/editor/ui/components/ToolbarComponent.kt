package com.video.demo.feature.editor.ui.components

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyRow
import androidx.compose.foundation.lazy.items
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp

data class ToolbarItem(val title: String, val iconRes: Int = 0, val action: () -> Unit)

@Composable
fun ToolbarComponent(
    items: List<ToolbarItem>,
    modifier: Modifier = Modifier
) {
    LazyRow(
        modifier = modifier
            .fillMaxWidth()
            .height(60.dp)
            .background(MaterialTheme.colorScheme.background),
        verticalAlignment = Alignment.CenterVertically,
        horizontalArrangement = Arrangement.SpaceEvenly
    ) {
        items(items) { item ->
            TextButton(onClick = item.action) {
                Text(item.title, color = MaterialTheme.colorScheme.onSurface)
            }
        }
    }
}
