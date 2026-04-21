#pragma once

#include <database/interface/database.h>
#include <memory>


// TODO: Make it singleton
class PostgreSQL : public Database_i {
public:
    ~PostgreSQL() override;

    static std::shared_ptr<Database_i> get();

    bool connect() override;
protected:
    inline static std::shared_ptr<Database_i> m_instance;

    PostgreSQL();
};