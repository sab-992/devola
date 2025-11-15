cmake_minimum_required(VERSION 3.16)

project(logic VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LOGIC_PATH "${CMAKE_CURRENT_LIST_DIR}")

add_executable (logic "${LOGIC_PATH}/main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET logic PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(logic PRIVATE CORE DATABASE SERVER)
target_include_directories(logic PRIVATE ${CORE_INCLUDES} ${DATABASE_INCLUDES} ${SERVER_INCLUDES})
