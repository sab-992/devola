if(NOT TARGET database_lib)
    set(lib_path "${CMAKE_CURRENT_LIST_DIR}/../lib")
    set(cmake_modules_path "${CMAKE_CURRENT_LIST_DIR}/../cmake")

    include("${cmake_modules_path}/CreateLibrary.cmake")
    include("${cmake_modules_path}/ExternalLibraries.cmake")

    FetchContent_MakeAvailable(libpqxx)

    create_library("${lib_path}/database" STATIC TRUE)
    target_link_libraries(database_lib PUBLIC libpqxx::pqxx)
endif()