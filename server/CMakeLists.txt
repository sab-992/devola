cmake_minimum_required(VERSION 3.16)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(cmake_modules_path "${CMAKE_CURRENT_LIST_DIR}/../cmake")

if(CMAKE_BUILD_TYPE STREQUAL "release" AND DEFINED SERVICE_NAME)
    # This block will be run for each services in its own Docker container (ONLY for Docker).
    include("${CMAKE_CURRENT_LIST_DIR}/service/${SERVICE_NAME}/CMakeLists.txt")
else()
    # This block will be run once. All services will be launched on the current device.
    include("${cmake_modules_path}/IncludeServices.cmake")
    include("${cmake_modules_path}/LinkAllLibraries.cmake")

    add_executable (dev_server "${CMAKE_CURRENT_LIST_DIR}/main.cpp")

    if (CMAKE_VERSION VERSION_GREATER 3.12)
        set_property(TARGET dev_server PROPERTY CXX_STANDARD 20)
    endif()

    include_services("${CMAKE_CURRENT_LIST_DIR}/service")
    link_all_libraries(dev_server "${CMAKE_CURRENT_LIST_DIR}/../lib")
endif()