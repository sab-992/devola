cmake_minimum_required(VERSION 3.16)

set(cmake_modules_path "${CMAKE_CURRENT_LIST_DIR}/../../../cmake")
set(authentication_path "${CMAKE_CURRENT_LIST_DIR}")

include("${cmake_modules_path}/FindCppFiles.cmake")
include("${cmake_modules_path}/DatabaseLibrary.cmake")

project(authentication VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_cpp_files("${authentication_path}/src" authentication_sources)

add_executable (authentication "${authentication_path}/main.cpp" ${authentication_sources})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET authentication PROPERTY CXX_STANDARD 20)
endif()

target_include_directories(authentication PRIVATE "${authentication_path}/include")
target_link_libraries(authentication PRIVATE core_lib server_lib database_lib)
