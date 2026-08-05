#pragma once

#include <database/query.hpp>
#include <database/result.hpp>
#include <database/transaction.hpp>


namespace database_n
{
    using objectID_t = std::string;

    class Creator_i {
    public:
        virtual ~Creator_i() = default;
        virtual Result Create(const Query& query, Transaction* transaction=nullptr) = 0;
    };

    class Reader_i {
    public:
        virtual ~Reader_i() = default;
        virtual Result Read(const Query& query, Transaction* transaction=nullptr) const = 0;
    };

    class Updater_i {
    public:
        virtual ~Updater_i() = default;
        virtual Result Update(const Query& query, Transaction* transaction=nullptr) = 0;
    };

    class Deleter_i {
    public:
        virtual ~Deleter_i() = default;
        virtual Result Delete(const Query& query, Transaction* transaction=nullptr) = 0;
    };

    class Other_i {
    public:
        virtual ~Other_i() = default;
        virtual Result Other(const Query& query, Transaction* transaction=nullptr) = 0;
    };
}