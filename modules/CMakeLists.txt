file(GLOB_RECURSE MODULES_SRC
    "camera/CameraCapture.cpp"
    "camera/CameraCaptureAndroid.cpp"
    "editor/*.cpp"
    "effect/*.cpp"
    "player/*.cpp"
)

add_library(VideoModules STATIC ${MODULES_SRC})
target_include_directories(VideoModules PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(VideoModules PRIVATE
    camera/CameraSourceNode.cpp
)
