# 独立为 Android JNI 编译时的 CMake 逻辑
cmake_minimum_required(VERSION 3.10.2)
project(VideoSDKAndroid CXX)

# 这里通常会通过 add_subdirectory 引入根目录的 C++ 核心代码
# 为了占位展示，仅将 JNI 目录打包
add_library(VideoSDKJni SHARED
    "jni/VideoEditorJni.cpp"
    "jni/VideoPlayerJni.cpp"
    "jni/CameraCaptureJni.cpp"
    "jni/TimelineExporterJni.cpp"
)
target_include_directories(VideoSDKJni PRIVATE ${CMAKE_SOURCE_DIR}/../../ ${CMAKE_SOURCE_DIR}/../../third_party/ndk_mock)
