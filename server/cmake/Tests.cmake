cmake_policy(SET CMP0135 NEW)


set(TEST_PATH "${PROJECT_SOURCE_DIR}/test")


find_CXX_files(TEST_SOURCES "${TEST_PATH}")
message(STATUS "Found TEST_SOURCES:")
foreach(source ${TEST_SOURCES})
    message(STATUS "  ${source}")
endforeach()


if(NOT TEST_SOURCES)
    message(WARNING "No test source files found, skipping rest of this file")
    return()
endif()

include(FetchContent)

FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip)

# Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(tests ${TEST_SOURCES})

target_link_libraries(tests PRIVATE CORE gtest_main gtest)

target_include_directories(tests PRIVATE ${CORE_INCLUDES})

if(WIN32)
    # Windows might need additional libraries
    target_link_libraries(tests ws2_32)
endif()

include(GoogleTest)
gtest_discover_tests(tests)