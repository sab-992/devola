cmake_minimum_required (VERSION 3.10)

project(devola VERSION 1.0)

set(BUILD_DIR "${PROJECT_SOURCE_DIR}/build")

add_subdirectory("src/authentication")
set_target_properties(authentication PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}/authentication")

add_subdirectory("src/voice")
set_target_properties(voice PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}/voice")
