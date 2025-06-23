cmake_minimum_required(VERSION 3.16)

project(authentication VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(AUTHENTICATION_PATH "${CMAKE_CURRENT_LIST_DIR}")

add_executable (authentication "${AUTHENTICATION_PATH}/main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET authentication PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(authentication PRIVATE CORE)
target_include_directories(authentication PRIVATE ${CORE_INCLUDES})
