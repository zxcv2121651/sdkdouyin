file(GLOB_RECURSE RHI_SRC
    "interface/*.cpp"
    "gles/*.cpp"
    "gles/EGLCore.cpp"
)

# conditional logic for other platforms
if(ANDROID)
    file(GLOB_RECURSE RHI_VULKAN_SRC "vulkan/*.cpp")
    list(APPEND RHI_SRC ${RHI_VULKAN_SRC})
elseif(APPLE)
    file(GLOB_RECURSE RHI_METAL_SRC "metal/*.cpp" "metal/*.mm")
    list(APPEND RHI_SRC ${RHI_METAL_SRC})
endif()

add_library(VideoRHI STATIC ${RHI_SRC})
target_include_directories(VideoRHI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/interface)
