#pragma once

#include <core/exception.hpp>
#include <core/file.hpp>
#include <core/logging.hpp>
#include <core/utility.hpp>
#include <database/interface/database.hpp>
#include <database/detail/postgre/options.hpp>
#include <database/detail/postgre/pool.hpp>
#include <memory>
#include <pqxx/pqxx>


class PostgreSQL : public database_n::Database_i {
    using Cardinality_en = database_n::Query::Cardinality_en;
    using ConnectionPool = pgsql_n::ConnectionPool;
    using json = nlohmann::json;
    using Options = pgsql_n::Options;
    using Query = database_n::Query;
    using record_t = database_n::record_t;
    using Result = database_n::Result;
    using Status_en = database_n::Status_en;
    using Value = database_n::Value;

    template<class... Ts>
    struct overloads : Ts... { using Ts::operator()...; };
    struct Private_s {};

    const std::vector<std::string> EXTRA_LOGS = { "PSQL" };
public:
    PostgreSQL(const Private_s&, const json& postgresJSON);

    PostgreSQL(const PostgreSQL&) = delete;
    PostgreSQL& operator=(const PostgreSQL&) = delete;

    PostgreSQL(PostgreSQL&&) = delete;
    PostgreSQL& operator=(PostgreSQL&&) = delete;

    ~PostgreSQL() = default;

    Result Create(const Query& query, Transaction* transaction=nullptr) override;
    Result Read(const Query& query, Transaction* transaction=nullptr) override;
    Result Update(const Query& query, Transaction* transaction=nullptr) override;
    Result Delete(const Query& query, Transaction* transaction=nullptr) override;
    Result Other(const Query& query, Transaction* transaction=nullptr) override;

    static std::shared_ptr<PostgreSQL> instance(const json& postgresJSON) {
        static std::shared_ptr<PostgreSQL> instance = std::make_shared<PostgreSQL>(Private_s(), postgresJSON);
        return instance;
    }

private:
    std::shared_ptr<log_n::Light> m_light;
    Options m_options;
    std::unique_ptr<pgsql_n::ConnectionPool> m_pool;

    void addParam(pqxx::params& params, const Value& value) const;
    pgsql_n::Transaction& beginTransaction(Transaction*& transaction, std::unique_ptr<Transaction>& ownedTransaction);
    std::string condition(const Query::filter_t& filter, pqxx::params& params, pqxx::placeholders<>& placeholders) const;
    Value convertField(pqxx::field const &field) const;
    std::pair<std::vector<std::string>, pqxx::params> extractParams(const record_t& record) const;
    std::string getProjection(const std::vector<std::string>& projection) const;
    std::string groupBy(const Query& query) const;
    std::string having(const Query& query, pqxx::params& params, pqxx::placeholders<>& placeholders) const;
    std::string limit(const Query& query) const;
    std::string offset(const Query& query) const;
    pgsql_n::Options optionsFromJSON(const json& postgresJSON) const;
    std::string orderBy(const Query& query) const;
    Result success(const pqxx::result& result) const;
    bool validateCardinality(const pqxx::result& result, Cardinality_en expected) const;
    std::string where(const Query& query, pqxx::params& params, pqxx::placeholders<>& placeholders) const;
};