# 独立为 Android JNI 编译时的 CMake 逻辑
cmake_minimum_required(VERSION 3.10.2)
project(VideoSDKAndroid CXX)

add_library(VideoSDKJni SHARED
    "jni/VideoEditorJni.cpp"
    "jni/CameraCaptureJni.cpp"
    "jni/VideoPlayerJni.cpp"
    "jni/TimelineExporterJni.cpp"
)

find_package(JNI)
if (JNI_FOUND)
    message(STATUS "JNI found.")
    target_include_directories(VideoSDKJni PRIVATE ${JNI_INCLUDE_DIRS})
endif()

target_include_directories(VideoSDKJni PRIVATE
    ${CMAKE_SOURCE_DIR}/../../
    ${CMAKE_SOURCE_DIR}/../../third_party/ndk_mock
)

if(ANDROID)
    target_link_libraries(VideoSDKJni PRIVATE android log)
endif()
