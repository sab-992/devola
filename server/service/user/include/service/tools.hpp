#pragma once

#include <database/interface/database.hpp>
#include <core/http.hpp>


namespace user
{
    struct ServerTools {
        std::shared_ptr<database_n::Database_i> database;
    };
}