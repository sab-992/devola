if(NOT TARGET database_lib)
    set(lib_path "${CMAKE_CURRENT_LIST_DIR}/../lib")
    set(cmake_modules_path "${CMAKE_CURRENT_LIST_DIR}/../cmake")

    include("${cmake_modules_path}/CreateLibrary.cmake")
    FetchContent_Declare(libpqxx GIT_REPOSITORY https://github.com/jtv/libpqxx.git GIT_TAG 7.10.1)

    FetchContent_MakeAvailable(libpqxx)

    create_library("${lib_path}/database" STATIC TRUE)
    target_link_libraries(database_lib PUBLIC libpqxx::pqxx)
endif()