
cmake_minimum_required (VERSION 3.10)

project(voice VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VOICE_PATH "${PROJECT_SOURCE_DIR}/src/voice")

add_executable (voice "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET voice PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(voice PRIVATE HTTP JSON LOGGER MEMORY PROCESS RPC THREADS)
target_include_directories(voice PRIVATE ${CORE_DIRECTORIES})
set_target_properties(voice PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_PATH}/voice")
