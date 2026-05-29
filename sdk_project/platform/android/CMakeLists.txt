# 独立为 Android JNI 编译时的 CMake 逻辑
cmake_minimum_required(VERSION 3.10.2)
project(VideoSDKAndroid CXX)

# 这里通常会通过 add_subdirectory 引入根目录的 C++ 核心代码
# 为了占位展示，仅将 JNI 目录打包
add_library(VideoSDKJni SHARED
    "jni/VideoEditorJni.cpp"
    "jni/CameraCaptureJni.cpp"
    "jni/VideoPlayerJni.cpp"
    "jni/TimelineExporterJni.cpp"
)

# 获取 JDK 的 JNI 头文件，或者我们项目里的 mock
find_package(JNI)
if (JNI_FOUND)
    message(STATUS "JNI found.")
    target_include_directories(VideoSDKJni PRIVATE ${JNI_INCLUDE_DIRS})
else()
    message(STATUS "JNI not found, using ndk_mock.")
    target_include_directories(VideoSDKJni PRIVATE ${CMAKE_SOURCE_DIR}/../../third_party/ndk_mock)
endif()

target_include_directories(VideoSDKJni PRIVATE
    ${CMAKE_SOURCE_DIR}/../../
)
