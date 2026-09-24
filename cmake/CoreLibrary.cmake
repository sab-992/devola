set(lib_path "${CMAKE_CURRENT_LIST_DIR}/../lib")
set(cmake_modules_path "${CMAKE_CURRENT_LIST_DIR}/../cmake")

include("${cmake_modules_path}/CreateLibrary.cmake")
include("${cmake_modules_path}/ExternalLibraries.cmake")

create_library("${lib_path}/core" SHARED FALSE)

target_include_directories(core_lib PUBLIC ${asio_SOURCE_DIR}/asio/include)

find_package(OpenSSL REQUIRED)
find_package(jwt-cpp CONFIG REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SODIUM REQUIRED libsodium)

target_include_directories(core_lib PRIVATE ${SODIUM_INCLUDE_DIRS})
target_compile_options(core_lib PRIVATE ${SODIUM_CFLAGS_OTHER})

target_link_libraries(core_lib PUBLIC nlohmann_json::nlohmann_json pugixml OpenSSL::SSL OpenSSL::Crypto jwt-cpp::jwt-cpp ${SODIUM_LIBRARIES})