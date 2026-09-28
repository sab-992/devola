cmake_minimum_required(VERSION 3.16)

set(cmake_modules_path "${CMAKE_CURRENT_LIST_DIR}/../../../cmake")
set(user_path "${CMAKE_CURRENT_LIST_DIR}")

include("${cmake_modules_path}/FindCppFiles.cmake")
include("${cmake_modules_path}/DatabaseLibrary.cmake")

project(user VERSION 1.0)

find_cpp_files("${user_path}/src" user_sources)

add_executable (user "${user_path}/main.cpp" ${user_sources})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET user PROPERTY CXX_STANDARD 20)
endif()

target_compile_definitions(user PRIVATE SERVICE_DIRECTORY="${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(user PRIVATE "${user_path}/include")
target_link_libraries(user PRIVATE core_lib database_lib)
