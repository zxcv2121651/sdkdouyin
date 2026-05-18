cmake_minimum_required(VERSION 3.10.2)
project(VideoSDK VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform detection
if(ANDROID)
    message(STATUS "Building for Android")
elseif(APPLE)
    message(STATUS "Building for Apple (iOS/macOS)")
endif()

# Global include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/ffmpeg_mock
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)

# Subdirectories
add_subdirectory(core)
add_subdirectory(rhi)
add_subdirectory(hal)
add_subdirectory(media)
add_subdirectory(modules)

# Build a dummy source for the main shared library so it isn't empty
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/dummy_core.cpp "void dummy_main_core(){}")

# Build the core shared library
add_library(VideoSDKCore SHARED ${CMAKE_CURRENT_BINARY_DIR}/dummy_core.cpp)

# Link sub-targets to the main library
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
