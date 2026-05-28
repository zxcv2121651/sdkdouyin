add_library(VideoHAL
    codec/DecoderPool.cpp
    codec/AndroidMediaCodecEncoder.cpp
    exporter/TimelineExporter.cpp
)

target_include_directories(VideoHAL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)

if(ANDROID)
    target_link_libraries(VideoHAL PRIVATE mediandk)
endif()
