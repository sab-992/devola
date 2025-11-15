cmake_minimum_required(VERSION 3.16)
project(devola VERSION 1.0)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(LIB_PATH "${CMAKE_CURRENT_LIST_DIR}/lib")
set(CMAKE_PATH "${PROJECT_SOURCE_DIR}/cmake")


include("${CMAKE_PATH}/External.cmake")
include("${CMAKE_PATH}/Functions.cmake")
include("${CMAKE_PATH}/Libraries.cmake")

if(CMAKE_BUILD_TYPE STREQUAL "Release" AND DEFINED SERVICE_NAME)
    # This block will be run for each services in its own Docker container (ONLY FOR Docker with linux).
    include("${CMAKE_PATH}/Release.cmake")
    include("src/${SERVICE_NAME}/CMakeLists.txt")
else()
    if (WIN32)
        set_target_properties(CORE PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
    endif()

    if(CMAKE_BUILD_TYPE STREQUAL "Tests")
        include("${CMAKE_PATH}/Tests.cmake")
    else()
        # This block will be run once. All services will be launched on the current device.
        include("${CMAKE_PATH}/Debug.cmake")
    endif()
endif()