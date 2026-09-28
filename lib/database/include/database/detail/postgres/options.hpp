#pragma once

#include <string>


namespace pgsql_n
{
    struct Options {
        std::string host;
        std::string port;
        std::string databaseName;
        std::string username;
        std::string password;
    };
}