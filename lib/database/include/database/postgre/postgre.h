#pragma once

#include <core/utility/singleton.h>
#include <database/interface/database.h>


class PostgreSQL : public Database_i, public Singleton<PostgreSQL> {
public:
    PostgreSQL(const Singleton<PostgreSQL>&);
    ~PostgreSQL() override;
};