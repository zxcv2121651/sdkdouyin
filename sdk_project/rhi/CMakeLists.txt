add_library(VideoRHI
    gles/EGLCore.cpp
    gles/GLESRenderer.cpp
    gles/EGLWindow.cpp
    vulkan/VulkanRenderer.cpp
)

target_include_directories(VideoRHI PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/interface
)

if(ANDROID)
    target_link_libraries(VideoRHI PRIVATE EGL GLESv3)
endif()
