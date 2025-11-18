find_files(CORE_SOURCES "${LIB_PATH}/core" "cpp")
find_includes(CORE_INCLUDES "${LIB_PATH}/core")

if(CORE_SOURCES)
  add_library(CORE SHARED ${CORE_SOURCES})
  if (CORE_INCLUDES)
    target_include_directories(CORE PUBLIC ${CORE_INCLUDES}
                                           ${asio_SOURCE_DIR}/asio/include)
    target_link_libraries(CORE PUBLIC libpqxx::pqxx
                                      nlohmann_json::nlohmann_json
                                      ixwebsocket)
  else()
    message(WARNING "No includes files found for CORE library")
  endif()
else()
    message(WARNING "No source files found for CORE library")
endif()

find_files(DATABASE_SOURCES "${LIB_PATH}/database" "cpp")
find_includes(DATABASE_INCLUDES "${LIB_PATH}/database")

add_library(DATABASE SHARED ${DATABASE_SOURCES})
target_include_directories(DATABASE PUBLIC ${DATABASE_INCLUDES})
target_link_libraries(DATABASE PUBLIC CORE)

find_files(SERVER_SOURCES "${LIB_PATH}/server" "cpp")
find_includes(SERVER_INCLUDES "${LIB_PATH}/server")

add_library(SERVER SHARED ${SERVER_SOURCES})
target_include_directories(SERVER PUBLIC ${SERVER_INCLUDES})
target_link_libraries(SERVER PUBLIC CORE)