cmake_minimum_required (VERSION 3.10)
project(devola VERSION 1.0)

include(FetchContent)
FetchContent_Declare(asio GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git GIT_TAG asio-1-34-2)


set(BUILD_PATH "${PROJECT_SOURCE_DIR}/build")
set(LIB_PATH "${PROJECT_SOURCE_DIR}/lib")
set(CORE_PATH "${LIB_PATH}/core")


set(HTTP_PATH "${CORE_PATH}/http")      
add_library(HTTP SHARED "${HTTP_PATH}/http.cpp")
target_include_directories(HTTP PRIVATE "${HTTP_PATH}")

set(JSON_PATH "${CORE_PATH}/json")      
add_library(JSON SHARED "${JSON_PATH}/json.cpp")
target_include_directories(JSON PRIVATE "${JSON_PATH}")

set(LOGGER_PATH "${CORE_PATH}/logger")      
add_library(LOGGER SHARED "${LOGGER_PATH}/logger.cpp")
target_include_directories(LOGGER PRIVATE "${LOGGER_PATH}")

set(MEMORY_PATH "${CORE_PATH}/memory")      
add_library(MEMORY SHARED "${MEMORY_PATH}/memory.cpp")
target_include_directories(MEMORY PRIVATE "${MEMORY_PATH}")

set(PROCESS_PATH "${CORE_PATH}/process")      
add_library(PROCESS SHARED "${PROCESS_PATH}/process.cpp")
target_include_directories(PROCESS PRIVATE "${PROCESS_PATH}")

set(RPC_PATH "${CORE_PATH}/rpc")      
add_library(RPC SHARED "${RPC_PATH}/rpc.cpp")
target_include_directories(RPC PRIVATE "${RPC_PATH}")

set(THREADS_PATH "${CORE_PATH}/threads")      
add_library(THREADS SHARED "${THREADS_PATH}/threads.cpp")
target_include_directories(THREADS PRIVATE "${THREADS_PATH}")

set(CORE_DIRECTORIES "${HTTP_PATH}"
                     "${JSON_PATH}"
                     "${LOGGER_PATH}"
                     "${MEMORY_PATH}"
                     "${PROCESS_PATH}"
                     "${RPC_PATH}"
                     "${THREADS_PATH}")

add_subdirectory("src/voice")

add_subdirectory("src/authentication")
