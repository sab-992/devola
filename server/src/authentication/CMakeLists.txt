
cmake_minimum_required (VERSION 3.10)

project(authentication VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(AUTHENTICATION_DIR "${PROJECT_SOURCE_DIR}/src/authentication")
set(LIB_DIR "${PROJECT_SOURCE_DIR}/lib")
set(CORE_DIR "${LIB_DIR}/core")

add_executable (authentication "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET authentication PROPERTY CXX_STANDARD 20)
endif()

target_include_directories(authentication PRIVATE ${CORE_DIR})
