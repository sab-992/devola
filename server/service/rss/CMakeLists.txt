cmake_minimum_required(VERSION 3.16)

set(cmake_modules_path "${CMAKE_CURRENT_LIST_DIR}/../../../cmake")
set(rss_path "${CMAKE_CURRENT_LIST_DIR}")

include("${cmake_modules_path}/FindCppFiles.cmake")


project(rss VERSION 1.0)

find_cpp_files("${rss_path}/src" rss_sources)

add_executable (rss "${rss_path}/main.cpp" ${rss_sources})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET rss PROPERTY CXX_STANDARD 20)
endif()

target_include_directories(rss PRIVATE "${rss_path}/include")
target_link_libraries(rss PRIVATE core_lib)
