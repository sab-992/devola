
cmake_minimum_required (VERSION 3.10)

project(voice VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VOICE_DIR "${PROJECT_SOURCE_DIR}/src/voice")
set(LIB_DIR "${PROJECT_SOURCE_DIR}/lib")
set(CORE_DIR "${LIB_DIR}/core")

add_executable (voice "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET voice PROPERTY CXX_STANDARD 20)
endif()

target_include_directories(voice PRIVATE ${CORE_DIR})
