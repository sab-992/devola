#pragma once

#include <database/interface/database.hpp>
#include <core/http.hpp>


namespace rss
{
    struct ServerTools {
        std::shared_ptr<database_n::Database_i> cache;
        std::shared_ptr<database_n::Database_i> database;
        const std::unique_ptr<http_n::Http>& http;
    };
}