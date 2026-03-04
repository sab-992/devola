set(lib_path "${CMAKE_CURRENT_LIST_DIR}/../lib")
set(cmake_modules_path "${CMAKE_CURRENT_LIST_DIR}/../cmake")

include("${cmake_modules_path}/CreateLibrary.cmake")
include("${cmake_modules_path}/ExternalLibraries.cmake")

create_library("${lib_path}/core" SHARED FALSE)
target_include_directories(core_lib PUBLIC ${asio_SOURCE_DIR}/asio/include)
target_link_libraries(core_lib PUBLIC nlohmann_json::nlohmann_json pugixml)