cmake_minimum_required(VERSION 3.10.2)
project(VideoSDKJni VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/ndk_mock
    ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/vulkan_mock
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../
)

add_library(VideoSDKJni SHARED
    jni/VideoPlayerJni.cpp
    jni/TimelineExporterJni.cpp
    jni/CameraCaptureJni.cpp
)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../build)

target_link_libraries(VideoSDKJni
    PRIVATE
    /app/build/libVideoSDKCore.so
)

if(ANDROID)
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../build)

target_link_libraries(VideoSDKJni PRIVATE log android EGL GLESv3)
endif()
