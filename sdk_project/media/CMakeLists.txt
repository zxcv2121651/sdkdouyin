file(GLOB_RECURSE MEDIA_SRC
    "ffmpeg/*.cpp"
    "codec/*.cpp"
    "sync/*.cpp"
)

add_library(VideoMedia STATIC ${MEDIA_SRC})
target_include_directories(VideoMedia PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(TestAVSync sync/TestAVSync.cpp)
target_link_libraries(TestAVSync PRIVATE VideoMedia VideoCore pthread)
