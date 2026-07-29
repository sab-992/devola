#pragma once

#include <database/detail/postgre/transaction.hpp>
#include <pqxx/pqxx>


class Transaction {
public:
    using variant_t = std::variant<std::monostate, pgsql_n::Transaction>;

    Transaction() = default;
    ~Transaction() = default;

    Transaction(pgsql_n::Transaction&& transaction) : m_variant(std::move(transaction)) {};

    Transaction(Transaction&& other) : m_variant(std::move(other.m_variant)) {}
    Transaction& operator=(Transaction&& other) {
        m_variant = std::move(other.m_variant);
        return *this;
    };

    template <typename T>
    T& get() { return std::get<T>(m_variant); }

    bool isNull() const {
        return std::holds_alternative<std::monostate>(m_variant);
    }

    const variant_t& raw() const {
        return m_variant;
    }

private:
    variant_t m_variant;
};