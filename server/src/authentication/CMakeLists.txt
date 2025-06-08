
cmake_minimum_required (VERSION 3.10)

project(authentication VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(AUTHENTICATION_PATH "${PROJECT_SOURCE_DIR}/src/authentication")

add_executable (authentication "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET authentication PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(authentication PRIVATE HTTP JSON LOGGER MEMORY PROCESS RPC THREADS)
target_include_directories(authentication PRIVATE ${CORE_DIRECTORIES})
set_target_properties(authentication PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_PATH}/authentication")
