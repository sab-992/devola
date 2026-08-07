#include <database/detail/postgres/pool.hpp>


// ----------------------------------------------------------
//                            ACCESS
// ----------------------------------------------------------

pgsql_n::ConnectionPool::Access::Access(ConnectionPool& pool) : m_pool(pool), m_connection(pool.acquire()) {}

pgsql_n::ConnectionPool::Access::~Access() {
    m_pool.release(std::move(m_connection));
}

pqxx::connection& pgsql_n::ConnectionPool::Access::connection() & {
    return *m_connection;
}

std::unique_ptr<pgsql_n::ConnectionPool::Access> pgsql_n::ConnectionPool::access() {
    return std::make_unique<Access>(*this);
}

// ----------------------------------------------------------
//                        ConnectionPool
// ----------------------------------------------------------

pgsql_n::ConnectionPool::ConnectionPool(const Options& options) {
    for (size_t i = 0; i < POOL_SIZE; ++i)
        m_pool.emplace(makeConnection(options));
}

pgsql_n::ConnectionPool::~ConnectionPool() {}

std::unique_ptr<pqxx::connection> pgsql_n::ConnectionPool::acquire() {
    std::unique_lock<std::mutex> lock(m_mutex);

    m_conditionVariable.wait(lock, [this] { return !m_pool.empty(); });
    std::unique_ptr<pqxx::connection> connection = std::move(m_pool.front());
    m_pool.pop();

    return connection;
}

void pgsql_n::ConnectionPool::release(std::unique_ptr<pqxx::connection> connection) {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_pool.push(std::move(connection));
    m_conditionVariable.notify_one();
}

std::unique_ptr<pqxx::connection> pgsql_n::ConnectionPool::makeConnection(const Options& options) {
    return std::make_unique<pqxx::connection>(std::format("host={} port={} dbname={} user={}{}", options.host,
                                                                                                    options.port,
                                                                                                    options.databaseName,
                                                                                                    options.username,
                                                                                                    trim(options.password).empty() ? "": std::format(" password={}", options.password)));
}