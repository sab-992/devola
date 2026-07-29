#pragma once

#include <condition_variable>
#include <core/str.hpp>
#include <database/detail/postgre/options.hpp>
#include <memory>
#include <mutex>
#include <pqxx/pqxx>
#include <queue>


namespace pgsql_n
{
    class ConnectionPool {
        const uint8_t POOL_SIZE = 25;

    public:
        ConnectionPool(const Options& options);
        ~ConnectionPool();

        friend std::unique_ptr<ConnectionPool> std::make_unique<ConnectionPool>();

        class Access {
        public:
            explicit Access(ConnectionPool& pool);
            Access(const Access&) = delete;
            Access& operator=(const Access&) = delete;
            ~Access();

            friend std::unique_ptr<Access> std::make_unique<Access>();

            pqxx::connection& connection() &;
            pqxx::connection& connection() && = delete;

        private:
            std::unique_ptr<pqxx::connection> m_connection;
            ConnectionPool& m_pool;
        };

        std::unique_ptr<Access> access();

    private:
        std::condition_variable m_conditionVariable;
        std::mutex m_mutex;
        std::queue<std::unique_ptr<pqxx::connection>> m_pool;

        std::unique_ptr<pqxx::connection> acquire();
        std::unique_ptr<pqxx::connection> makeConnection(const Options& options);
        void release(std::unique_ptr<pqxx::connection> connection);
    };
}