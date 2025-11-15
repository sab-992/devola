find_CXX_files(CORE_SOURCES "${LIB_PATH}/core")
find_includes(CORE_INCLUDES "${LIB_PATH}/core")

if(CORE_SOURCES)
  add_library(CORE SHARED ${CORE_SOURCES})
  if (CORE_INCLUDES)
    target_include_directories(CORE PUBLIC ${CORE_INCLUDES}
                                           ${asio_SOURCE_DIR}/asio/include)
    target_link_libraries(CORE PUBLIC libpqxx::pqxx
                                      nlohmann_json::nlohmann_json)
  else()
    message(WARNING "No includes files found for CORE library")
  endif()
else()
    message(WARNING "No source files found for CORE library")
endif()

find_CXX_files(DATABASE_SOURCES "${LIB_PATH}/database")
find_includes(DATABASE_INCLUDES "${LIB_PATH}/database")

add_library(DATABASE SHARED ${DATABASE_SOURCES})
target_include_directories(DATABASE PUBLIC ${DATABASE_INCLUDES})
target_link_libraries(DATABASE PUBLIC CORE)

find_CXX_files(SERVER_SOURCES "${LIB_PATH}/server")
find_includes(SERVER_INCLUDES "${LIB_PATH}/server")

add_library(SERVER SHARED ${SERVER_SOURCES})
target_include_directories(SERVER PUBLIC ${SERVER_INCLUDES})
target_link_libraries(SERVER PUBLIC CORE)