file(GLOB_RECURSE CORE_SRC
    "engine/*.cpp"
    "engine/RenderNode.cpp"
    "engine/RenderGraph.cpp"
    "engine/GlMock.cpp"
    "engine/OesToRgbFilter.cpp"
    "timeline/*.cpp"
    "audio/*.cpp"
    "audio/AudioMixer.cpp"
    "utils/*.cpp"
    "math/*.cpp"
)

# We might not have files yet, so handle the empty case

add_library(VideoCore STATIC ${CORE_SRC})
target_include_directories(VideoCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(TestRenderGraph engine/TestRenderGraph.cpp)
target_link_libraries(TestRenderGraph PRIVATE VideoCore)
target_sources(VideoCore PRIVATE
    c_api/video_sdk_c_api_impl.cpp
)
target_sources(VideoCore PRIVATE
    utils/RenderThread.cpp
)
add_executable(TestRenderThread engine/TestRenderThread.cpp)
target_link_libraries(TestRenderThread PRIVATE VideoCore VideoRHI)
target_sources(VideoCore PRIVATE
    nle/NLEModel.cpp
    nle/NLEPreloadManager.cpp
    nle/NLECompiler.cpp
)
add_executable(TestNLECompiler nle/TestNLECompiler.cpp)
target_link_libraries(TestNLECompiler PRIVATE VideoCore VideoRHI)
