#pragma once

#include <condition_variable>
#include <core/str.hpp>
#include <database/detail/redis/options.hpp>
#include <hiredis/hiredis.h>
#include <memory>
#include <mutex>
#include <queue>


namespace redis_n
{
    struct RedisContextDeleter {
        void operator()(redisContext* connection) const {
            if (connection)
                redisFree(connection);
        }
    };

    using ConnectionPtr = std::unique_ptr<redisContext, RedisContextDeleter>;

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

            redisContext* connection() &;
            redisContext* connection() && = delete;

        private:
            ConnectionPtr m_connection;
            ConnectionPool& m_pool;
        };

        std::unique_ptr<Access> access();

    private:
        std::condition_variable m_conditionVariable;
        std::mutex m_mutex;
        std::queue<ConnectionPtr> m_pool;

        ConnectionPtr acquire();
        ConnectionPtr makeConnection(const Options& options);
        void release(ConnectionPtr connection);
    };
}