cmake_minimum_required(VERSION 3.16)
project(devola VERSION 1.0)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(BUILD_PATH "${PROJECT_SOURCE_DIR}/build")
set(LIB_PATH "${CMAKE_CURRENT_LIST_DIR}/lib")
set(CMAKE_PATH "${PROJECT_SOURCE_DIR}/cmake")


include("${CMAKE_PATH}/External.cmake")

include("${CMAKE_PATH}/Lib.cmake")
FindCXXFiles(CORE_SOURCES "${LIB_PATH}/core")
FindIncludes(CORE_INCLUDES "${LIB_PATH}/core")

if(CORE_SOURCES)
  add_library(CORE SHARED ${CORE_SOURCES})
  if (CORE_INCLUDES)
    target_include_directories(CORE PUBLIC ${CORE_INCLUDES}
                                           ${asio_SOURCE_DIR}/asio/include)
    target_link_libraries(CORE PUBLIC 
    libpqxx::pqxx
    nlohmann_json::nlohmann_json
  )
  else()
    message(WARNING "No includes files found for CORE library")
  endif()
else()
    message(WARNING "No source files found for CORE library")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release" AND DEFINED SERVICE_NAME)
    # This block will be run for each services in its own Docker container.
    include("${CMAKE_PATH}/Release.cmake")
    include("src/${SERVICE_NAME}/CMakeLists.txt")
else()
    # This block will be run once. All services will be launched on the current device.
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    include("${CMAKE_PATH}/Debug.cmake")
endif()