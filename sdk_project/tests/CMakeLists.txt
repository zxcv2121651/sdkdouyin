add_executable(TestRenderGraph TestRenderGraph.cpp)
add_executable(TestAVSync TestAVSync.cpp)

target_link_libraries(TestRenderGraph sdk_core sdk_rhi)
target_link_libraries(TestAVSync sdk_core sdk_media)
