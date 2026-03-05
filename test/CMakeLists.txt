cmake_policy(SET CMP0135 NEW)

set(cmake_modules_path "${CMAKE_CURRENT_LIST_DIR}/../cmake")

include("${cmake_modules_path}/FindCppFiles.cmake")
include("${cmake_modules_path}/LinkAllLibraries.cmake")

include(FetchContent)
FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip)
# Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests) 

find_cpp_files("${CMAKE_CURRENT_LIST_DIR}" test_sources)
add_executable(tests ${test_sources})

target_link_libraries(tests PRIVATE gtest_main gtest)
link_all_libraries(tests "${CMAKE_CURRENT_LIST_DIR}/../lib")

if(WIN32)
    # Windows might need additional libraries
    target_link_libraries(tests ws2_32)
endif()

include(GoogleTest)
gtest_discover_tests(tests)
add_custom_target(run_all_tests COMMAND tests --gtest_color=yes
                                DEPENDS tests
                                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                                USES_TERMINAL)