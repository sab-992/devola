set(lib_path "${CMAKE_CURRENT_LIST_DIR}/../lib")
set(cmake_modules_path "${CMAKE_CURRENT_LIST_DIR}/../cmake")

include("${cmake_modules_path}/CreateLibrary.cmake")
include("${cmake_modules_path}/ExternalLibraries.cmake")

# Temporary solution for DLLS not being found correctly on windows.
if (WIN32)
    create_library("${lib_path}/core" STATIC FALSE)
else()
    create_library("${lib_path}/core" SHARED FALSE)
endif()

target_include_directories(core_lib PUBLIC ${asio_SOURCE_DIR}/asio/include)

find_package(OpenSSL REQUIRED)
target_link_libraries(core_lib PUBLIC nlohmann_json::nlohmann_json pugixml OpenSSL::SSL OpenSSL::Crypto)