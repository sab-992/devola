#include <database/postgre/postgre.h>


PostgreSQL::PostgreSQL() {};
PostgreSQL::~PostgreSQL() {};

bool PostgreSQL::connect() {  return true; };

std::shared_ptr<Database_i> PostgreSQL::get() { 
    if (m_instance == nullptr)
        m_instance = std::shared_ptr<Database_i>(new PostgreSQL());
    return m_instance;
};