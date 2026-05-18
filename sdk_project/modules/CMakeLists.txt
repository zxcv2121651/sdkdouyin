file(GLOB_RECURSE MODULES_SRC
    "camera/*.cpp"
    "editor/*.cpp"
    "effect/*.cpp"
)

if(NOT MODULES_SRC)
    set(MODULES_SRC "dummy.cpp")
    file(WRITE dummy.cpp "void modules_dummy(){}")
endif()

add_library(VideoModules STATIC ${MODULES_SRC})
target_include_directories(VideoModules PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
