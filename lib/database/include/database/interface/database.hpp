#pragma once

#include <database/interface/connection.hpp>
#include <database/interface/operations.hpp>


namespace database_n
{
    class Database_i : public Creator_i,
                       public Reader_i,
                       public Updater_i,
                       public Deleter_i {
    public:
        virtual ~Database_i() = default;
    };
}