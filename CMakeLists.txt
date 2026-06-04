cmake_minimum_required(VERSION 3.20)

project(devola VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(RUN_TESTS "Build tests" OFF)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/lib")
add_compile_definitions($<$<CONFIG:Debug>:DEBUG_MODE_ENABLED>)

if(ENABLE_TESTS)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test")
else()
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/server")
endif()
