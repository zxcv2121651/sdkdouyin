file(GLOB_RECURSE HAL_SRC
    "interface/*.cpp"
    "codec/*.cpp"
    "exporter/*.cpp"
)

if(NOT HAL_SRC)
    set(HAL_SRC "dummy.cpp")
    file(WRITE dummy.cpp "void hal_dummy(){}")
endif()

add_library(VideoHAL STATIC ${HAL_SRC})
target_include_directories(VideoHAL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/interface)
