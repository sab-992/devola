cmake_minimum_required(VERSION 3.16)
project(devola VERSION 1.0)

include(FetchContent)
FetchContent_Declare(asio GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git GIT_TAG asio-1-34-2)
FetchContent_Declare(libpqxx GIT_REPOSITORY https://github.com/jtv/libpqxx.git GIT_TAG 7.10.1)
FetchContent_Declare(nlohmann_json GIT_REPOSITORY https://github.com/nlohmann/json.git GIT_TAG v3.12.0)
FetchContent_Declare(mongocxx GIT_REPOSITORY https://github.com/mongodb/mongo-cxx-driver.git GIT_TAG r4.1.0)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")


set(BUILD_PATH "${PROJECT_SOURCE_DIR}/build")
set(LIB_PATH "${PROJECT_SOURCE_DIR}/lib")


function(find_cpp_files result_var search_path)
  file(GLOB_RECURSE all_files "${search_path}/*.cpp")
  set(filtered_files "")
  
  foreach(file ${all_files})
    get_filename_component(file_dir ${file} DIRECTORY)
    get_filename_component(dir_name ${file_dir} NAME)
    
    if(NOT dir_name STREQUAL "include")
      list(APPEND filtered_files ${file})
    endif()
  endforeach()
  
  set(${result_var} ${filtered_files} PARENT_SCOPE)
endfunction()

function(find_include_dirs result_var search_path)
  file(GLOB_RECURSE all_files "${search_path}/*")
  set(include_dirs "")
  foreach(file ${all_files})
    get_filename_component(file_dir ${file} DIRECTORY)
    get_filename_component(dir_name ${file_dir} NAME)
    if(dir_name STREQUAL "include")
      list(APPEND include_dirs ${file_dir})
    endif()
  endforeach()
  list(REMOVE_DUPLICATES include_dirs)
  set(${result_var} ${include_dirs} PARENT_SCOPE)
endfunction()

find_cpp_files(CORE_SOURCES "${LIB_PATH}/core")
find_include_dirs(CORE_INCLUDES "${LIB_PATH}/core")

message(STATUS "Found CORE sources:")
foreach(source ${CORE_SOURCES})
    message(STATUS "  ${source}")
endforeach()

message(STATUS "Found CORE includes:")
foreach(include ${CORE_INCLUDES})
    message(STATUS "  ${include}")
endforeach()

if(CORE_SOURCES)
  add_library(CORE SHARED ${CORE_SOURCES})
  if (CORE_INCLUDES)
    target_include_directories(CORE PUBLIC ${CORE_INCLUDES})
  else()
    message(WARNING "No includes files found for CORE library")
  endif()
else()
    message(WARNING "No source files found for CORE library")
endif()

add_executable (devola "${PROJECT_SOURCE_DIR}/main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET devola PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(devola PRIVATE CORE)
target_include_directories(devola PRIVATE ${CORE_INCLUDES})


add_subdirectory("src/voice")

add_subdirectory("src/authentication")