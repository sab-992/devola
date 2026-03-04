include(FetchContent)

set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_LIST_DIR}/../build/lib)

FetchContent_Declare(asio GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git GIT_TAG asio-1-34-2)
FetchContent_Declare(pugixml GIT_REPOSITORY https://github.com/zeux/pugixml.git GIT_TAG v1.15)
FetchContent_Declare(nlohmann_json GIT_REPOSITORY https://github.com/nlohmann/json.git GIT_TAG v3.12.0)
FetchContent_Declare(libpqxx GIT_REPOSITORY https://github.com/jtv/libpqxx.git GIT_TAG 7.10.1)
# Add NoSQL database to store big data.

FetchContent_MakeAvailable(asio nlohmann_json pugixml)