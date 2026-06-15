#pragma once

#include <core/utility/singleton.hpp>
#include <database/interface/database.hpp>


class PostgreSQL : public Database_i, public Singleton<PostgreSQL> {
public:
    PostgreSQL(const Singleton<PostgreSQL>&);
    ~PostgreSQL() = default;
};