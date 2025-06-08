cmake_minimum_required (VERSION 3.10)
project(devola VERSION 1.0)

include(FetchContent)
FetchContent_Declare(asio GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git GIT_TAG asio-1-34-2)


set(BUILD_DIR "${PROJECT_SOURCE_DIR}/build")
set(LIB_DIR "${PROJECT_SOURCE_DIR}/lib")
set(CORE_DIR "${LIB_DIR}/core")


add_library(HTTP SHARED "${CORE_DIR}/http/http.cpp")
target_include_directories(HTTP PRIVATE "${CORE_DIR}/http")

add_subdirectory("src/authentication")
set_target_properties(authentication PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}/authentication")

add_subdirectory("src/voice")
set_target_properties(voice PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}/voice")
