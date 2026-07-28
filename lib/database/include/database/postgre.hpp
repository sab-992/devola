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

public:
    PostgreSQL(const Private_s&, const json& postgresJSON);

    PostgreSQL(const PostgreSQL&) = delete;
    PostgreSQL& operator=(const PostgreSQL&) = delete;

    PostgreSQL(PostgreSQL&&) = delete;
    PostgreSQL& operator=(PostgreSQL&&) = delete;

    ~PostgreSQL() = default;

    Result Create(const Query& query) override;
    Result Read(const Query& query) const override;
    Result Update(const Query& query) override;
    Result Delete(const Query& query) override;

    static std::shared_ptr<PostgreSQL> instance(const json& postgresJSON) {
        static std::shared_ptr<PostgreSQL> instance = std::make_shared<PostgreSQL>(Private_s(), postgresJSON);
        return instance;
    }

private:
    const std::vector<std::string> m_extraLogs = { "PSQL" };
    std::shared_ptr<log_n::Light> m_light;
    Options m_options;
    std::unique_ptr<pgsql_n::ConnectionPool> m_pool;

    Value convertField(pqxx::field const &field) const;
    std::pair<std::vector<std::string>, pqxx::params> extractParams(const record_t& record) const;
    std::string getProjection(const std::vector<std::string>& projection) const;
    pgsql_n::Options optionsFromJSON(const json& postgresJSON) const;
    Result success(const pqxx::result& result) const;
    bool validateCardinality(const pqxx::result& result, Cardinality_en expected) const;
};