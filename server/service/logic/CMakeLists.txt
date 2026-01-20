cmake_minimum_required(VERSION 3.16)

set(cmake_modules_path "${CMAKE_CURRENT_LIST_DIR}/../../../cmake")
set(logic_path "${CMAKE_CURRENT_LIST_DIR}")

include("${cmake_modules_path}/FindCppFiles.cmake")
include("${cmake_modules_path}/DatabaseLibrary.cmake")

project(logic VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_cpp_files("${logic_path}/src" logic_sources)

add_executable (logic "${logic_path}/main.cpp" ${logic_sources})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET logic PROPERTY CXX_STANDARD 20)
endif()

target_include_directories(logic PRIVATE "${logic_path}/include")
target_link_libraries(logic PRIVATE core_lib server_lib database_lib)
