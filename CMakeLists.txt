cmake_minimum_required(VERSION 3.20)

project(devola VERSION 1.0)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/lib")

if(CMAKE_BUILD_TYPE STREQUAL "test")
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test")
else()
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/server")
endif()
