file(GLOB_RECURSE CORE_SRC
    "engine/*.cpp"
    "timeline/*.cpp"
    "audio/*.cpp"
    "utils/*.cpp"
    "c_api/*.cpp"
)

add_library(VideoCore STATIC ${CORE_SRC})
target_include_directories(VideoCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(VideoCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(TestRenderGraph engine/TestRenderGraph.cpp)
target_link_libraries(TestRenderGraph PRIVATE VideoCore)
