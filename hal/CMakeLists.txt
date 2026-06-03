file(GLOB_RECURSE HAL_SRC
    "interface/*.cpp"
    "codec/*.cpp"
    "exporter/*.cpp"
)

add_library(VideoHAL STATIC ${HAL_SRC})
target_include_directories(VideoHAL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/interface)

if(ANDROID)
    target_link_libraries(VideoHAL PUBLIC mediandk android log)
endif()
