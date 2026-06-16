cmake_minimum_required(VERSION 3.10.2)
project(VideoSDK VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(ANDROID)
    message(STATUS "Building for Android")
elseif(APPLE)
    message(STATUS "Building for Apple (iOS/macOS)")
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/ndk_mock
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/vulkan_mock
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/ffmpeg_mock
)

add_subdirectory(core)
add_subdirectory(rhi)
add_subdirectory(hal)
add_subdirectory(media)
add_subdirectory(modules)

# 为了创建一个不报错的 SHARED 库，我们需要提供至少一个源文件，创建一个正式的入口文件
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/VideoSDKEntry.cpp "
#include <iostream>
void video_sdk_init() { std::cout << \"Video SDK Initialized\" << std::endl; }
")

add_library(VideoSDKCore SHARED ${CMAKE_CURRENT_BINARY_DIR}/VideoSDKEntry.cpp)

target_link_libraries(VideoSDKCore
    PRIVATE
    VideoCore
    VideoRHI
    VideoHAL
    VideoMedia
    VideoModules
)

if(ANDROID)
    target_link_libraries(VideoSDKCore PRIVATE log android EGL GLESv3)
endif()
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
