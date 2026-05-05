#pragma once

#include <core/utility/interface/singleton.h>
#include <database/interface/database.h>
#include <memory>


class PostgreSQL : public Database_i, public Singleton<PostgreSQL> {
public:
    PostgreSQL(const Singleton<PostgreSQL>&);
    ~PostgreSQL() override;
};