#include <database/detail/redis/pool.hpp>



// ----------------------------------------------------------
//                            Access
// ----------------------------------------------------------

redis_n::ConnectionPool::Access::Access(ConnectionPool& pool) : m_pool(pool), m_connection(pool.acquire()) {}

redis_n::ConnectionPool::Access::~Access() {
    m_pool.release(std::move(m_connection));
}

redisContext* redis_n::ConnectionPool::Access::connection() & {
    return m_connection.get();
}

std::unique_ptr<redis_n::ConnectionPool::Access> redis_n::ConnectionPool::access() {
    return std::make_unique<Access>(*this);
}

// ----------------------------------------------------------
//                        ConnectionPool
// ----------------------------------------------------------

redis_n::ConnectionPool::ConnectionPool(const Options& options) {
    for (size_t i = 0; i < POOL_SIZE; ++i)
        m_pool.emplace(makeConnection(options));
}

redis_n::ConnectionPool::~ConnectionPool() {}

redis_n::ConnectionPtr redis_n::ConnectionPool::acquire() {
    std::unique_lock<std::mutex> lock(m_mutex);

    m_conditionVariable.wait(lock, [this] { return !m_pool.empty(); });
    ConnectionPtr connection = std::move(m_pool.front());
    m_pool.pop();

    return connection;
}

void redis_n::ConnectionPool::release(ConnectionPtr connection) {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_pool.push(std::move(connection));
    m_conditionVariable.notify_one();
}

redis_n::ConnectionPtr redis_n::ConnectionPool::makeConnection(const Options& options) {
    ConnectionPtr connection(redisConnect(options.host.c_str(), options.port));

    if (not connection)
        throw std::runtime_error("Failed to allocate redis connection");
    if (connection->err)
        throw std::runtime_error(std::format("Failed to connect to redis: {}", connection->errstr));

    if (not trim(options.password).empty()) {
        redisReply* reply = static_cast<redisReply*>(
            redisCommand(connection.get(), "AUTH %s", options.password.c_str()));

        if (not reply)
            throw std::runtime_error(std::format("Redis AUTH failed: {}", connection->errstr));

        bool failed = reply->type == REDIS_REPLY_ERROR;
        std::string error = failed ? reply->str : "";
        freeReplyObject(reply);

        if (failed)
            throw std::runtime_error(std::format("Redis AUTH failed: {}", error));
    }

    if (options.database != 0) {
        redisReply* reply = static_cast<redisReply*>(
            redisCommand(connection.get(), "SELECT %d", options.database));

        if (not reply)
            throw std::runtime_error(std::format("Redis SELECT failed: {}", connection->errstr));

        bool failed = reply->type == REDIS_REPLY_ERROR;
        std::string error = failed ? reply->str : "";
        freeReplyObject(reply);

        if (failed)
            throw std::runtime_error(std::format("Redis SELECT failed: {}", error));
    }

    return connection;
}