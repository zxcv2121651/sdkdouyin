file(GLOB_RECURSE HAL_SRC
    "interface/*.cpp"
    "codec/*.cpp"
    "codec/DecoderPool.cpp"
    "exporter/*.cpp"
    "exporter/TimelineExporter.cpp"
)


add_library(VideoHAL STATIC ${HAL_SRC})
target_include_directories(VideoHAL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/interface)
