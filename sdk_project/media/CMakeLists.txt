file(GLOB_RECURSE MEDIA_SRC
    "ffmpeg/*.cpp"
    "codec/*.cpp"
    "sync/*.cpp"
)

if(NOT MEDIA_SRC)
    set(MEDIA_SRC "dummy.cpp")
    file(WRITE dummy.cpp "void media_dummy(){}")
endif()

add_library(VideoMedia STATIC ${MEDIA_SRC})
target_include_directories(VideoMedia PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
