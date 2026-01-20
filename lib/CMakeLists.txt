cmake_minimum_required(VERSION 3.16)

set(cmake_modules_path "${CMAKE_CURRENT_LIST_DIR}/../cmake")
set(build_path "${CMAKE_CURRENT_LIST_DIR}/../build")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${build_path})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${build_path}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${build_path}/lib)

include("${cmake_modules_path}/CoreLibrary.cmake")
include("${cmake_modules_path}/CreateLibraries.cmake")

create_libraries(${lib_path})