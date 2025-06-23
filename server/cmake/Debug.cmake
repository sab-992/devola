message(STATUS "Found CORE sources:")
foreach(source ${CORE_SOURCES})
    message(STATUS "  ${source}")
endforeach()

message(STATUS "Found CORE includes:")
foreach(include ${CORE_INCLUDES})
    message(STATUS "  ${include}")
endforeach()

add_executable (devola "${PROJECT_SOURCE_DIR}/main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET devola PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(devola PRIVATE CORE)
target_include_directories(devola PRIVATE ${CORE_INCLUDES})

include("${CMAKE_PATH}/Services.cmake")
IncludeAllServices()