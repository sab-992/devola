#include <Postgre.h>

PostgreSQL::PostgreSQL() {};
PostgreSQL::~PostgreSQL() {};

bool PostgreSQL::Connect() { 
    std::cout << "Connecting ..." << std::endl; 
    return true;
};

std::shared_ptr<Database_i> PostgreSQL::Create() { 
    if (m_Instance == nullptr)
        m_Instance = std::shared_ptr<Database_i>(new PostgreSQL());
    return m_Instance;
};