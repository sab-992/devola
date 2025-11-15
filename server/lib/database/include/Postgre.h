#pragma once

#include <iostream>
#include <Database.h>
#include <memory>

class PostgreSQL : public Database_i {
public:
    ~PostgreSQL() override;

    static std::shared_ptr<Database_i> Create();

    bool Connect() override;
protected:
    inline static std::shared_ptr<Database_i> m_Instance;

    PostgreSQL();
};