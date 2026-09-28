#pragma once

#include <database/detail/postgres/pool.hpp>
#include <database/detail/tx.hpp>
#include <pqxx/pqxx>


namespace pgsql_n
{
    class Transaction : public database_n::Tx {
        using ConnectionPool = pgsql_n::ConnectionPool;

    public:
        Transaction() = default;
        ~Transaction() {
            abort();
        }

        Transaction(Transaction&& other) : m_access(std::move(other.m_access)), m_tx(std::move(other.m_tx)) {}
        Transaction& operator=(Transaction&& other) {
            m_access = std::move(other.m_access);
            m_tx = std::move(other.m_tx);
            return *this;
        };

        friend std::unique_ptr<Transaction> std::make_unique<Transaction>();

        void abort() {
            if (not m_tx) return;

            m_tx->abort();
            m_tx = nullptr;
        }

        void begin(pgsql_n::ConnectionPool& pool) {
            if (m_tx) return;

            m_access = pool.access();

            if (not m_access->connection().is_open())
                throw Exception("Couldn't connect to the database");

            m_tx = std::make_unique<pqxx::work>(m_access->connection());
        }

        void commit() {
            if (not m_tx)
                throw LogicException("Transaction: \"m_tx\" is nullptr");

            m_tx->commit();
            m_tx = nullptr;
        }

        pqxx::result execute(std::string_view sql, const pqxx::params& params) {
            return m_tx->exec(sql, params);
        }

    private:
        std::unique_ptr<pgsql_n::ConnectionPool::Access> m_access;
        std::unique_ptr<pqxx::work> m_tx;
    };
}