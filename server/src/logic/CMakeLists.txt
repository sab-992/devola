cmake_minimum_required(VERSION 3.16)

project(logic VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LOGIC_PATH "${CMAKE_CURRENT_LIST_DIR}")

find_files(LOGIC_SOURCES ${LOGIC_PATH} "cpp")
add_executable (logic ${LOGIC_SOURCES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET logic PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(logic PRIVATE CORE DATABASE SERVER)

find_includes(LOGIC_INCLUDES ${LOGIC_PATH})
target_include_directories(logic PRIVATE ${CORE_INCLUDES} ${LOGIC_INCLUDES} ${DATABASE_INCLUDES} ${SERVER_INCLUDES})

