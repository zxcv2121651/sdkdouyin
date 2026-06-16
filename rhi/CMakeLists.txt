file(GLOB_RECURSE RHI_SRC
    "*.cpp"
    "interface/*.cpp"
    "gles/*.cpp"
    "vulkan/*.cpp"
)

# 强制为所有平台编译 Vulkan 代码验证语法
add_library(VideoRHI STATIC ${RHI_SRC})
target_include_directories(VideoRHI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/interface)
target_sources(VideoRHI PRIVATE
)
