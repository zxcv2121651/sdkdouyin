file(GLOB_RECURSE CORE_SRC
    "engine/*.cpp"
    "timeline/*.cpp"
    "audio/*.cpp"
    "utils/*.cpp"
    "math/*.cpp"
)

# We might not have files yet, so handle the empty case
if(NOT CORE_SRC)
    set(CORE_SRC "dummy.cpp")
    file(WRITE dummy.cpp "void core_dummy(){}")
endif()

add_library(VideoCore STATIC ${CORE_SRC})
target_include_directories(VideoCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
