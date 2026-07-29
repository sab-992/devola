#include <database/postgre.hpp>


PostgreSQL::PostgreSQL(const Private_s&, const json& postgresJSON) : m_light(log_n::Light::instance()) {
    m_options = optionsFromJSON(postgresJSON);
    m_pool = std::make_unique<ConnectionPool>(m_options);
};

database_n::Value PostgreSQL::convertField(pqxx::field const &field) const {
    if (field.is_null())
        return nullptr;

    switch (field.type()) {
        case 16:   // bool
            return Value(field.as<bool>());
        case 20:   // int8
        case 21:   // int2
        case 23:   // int4
            return Value(field.as<int64_t>());
        case 700:  // float4
        case 701:  // float8
        case 1700: // numeric
            return Value(field.as<double>());
        default:   // text, varchar, date, timestamp, json, etc.
            return Value(field.as<std::string>());
    }
}

database_n::Result PostgreSQL::Create(const Query& query, Transaction* transaction) {
    std::unique_ptr<Transaction> ownedTransaction;

    if (not transaction) {
        ownedTransaction = std::make_unique<Transaction>(pgsql_n::Transaction());
        transaction = ownedTransaction.get();
    }

    pgsql_n::Transaction& tx = transaction->get<pgsql_n::Transaction>();
    tx.begin(*m_pool);
    try {
        if (not query.data().has_value())
            return Result().setError("No data to insert")
                           .setStatus(Status_en::QUERY_ERROR).build();

        const auto& [columns, params] = extractParams(query.data().value());

        std::string stringColumns;
        std::string values;

        for (size_t i = 0; i < columns.size(); i++) {
            if (i != 0) {
                stringColumns.append(", ");
                values.append(", ");
            }

            stringColumns.append(columns[i]);
            values.append(std::format("${}", i + 1));
        }

        std::string sql = std::format("INSERT INTO {} ({}) VALUES ({})", query.target(), stringColumns, values);

        if (query.projection().has_value())
            sql.append(std::format(" RETURNING {}", getProjection(query.projection().value())));

        pqxx::result queryResult = tx.execute(sql, params);

        if (not validateCardinality(queryResult, query.cardinality()))
            return Result().setError(std::format("Expected ({} rows) but got: ({} rows)", to_underlying(query.cardinality()), queryResult.size()))
                           .setStatus(Status_en::CARDINALITY_ERROR).build();

        if (ownedTransaction)
            tx.commit();

        return success(queryResult);
    } catch (std::exception& e) {
        tx.abort();
        m_light->log(log_n::Level_en::ERROR, m_extraLogs, "ABORTED: error during insertion: ", e.what());
        return Result().setError(e.what())
                       .setStatus(Status_en::QUERY_ERROR).build();
    }
}

database_n::Result PostgreSQL::Delete(const Query& query, Transaction* transaction) {
    return {};
}

std::pair<std::vector<std::string>, pqxx::params> PostgreSQL::extractParams(const record_t& record) const {
    std::vector<std::string> columns;
    pqxx::params params;

    for (const auto& [column, value] : record) {
        columns.emplace_back(column);

        std::visit(overloads{
            [&](const std::shared_ptr<record_t>&) { throw LogicException(std::format("Value for column '{}' is a nested record. It cannot be bound as Postgres params", column)); },
            [&](const std::shared_ptr<std::vector<Value>>&) { throw LogicException(std::format("Value for column '{}' is a nested list of values. It cannot be bound as Postgres params", column)); },
            [&, &value = std::as_const(value)](auto&& arg) {
                if (value.isNull())
                    params.append();
                else
                    params.append(arg);
            }
        }, value.raw());
    }

    return { std::move(columns), std::move(params) };
}

std::string PostgreSQL::getProjection(const std::vector<std::string>& projection) const {
    if (projection.empty())
        throw Exception("Projection exists but is empty");

    std::string columns;
    for (size_t i = 0; i < projection.size(); i++) {
        if (i != 0)
            columns.append(", ");

        columns.append(projection[i]);
    }

    return columns;
}

pgsql_n::Options PostgreSQL::optionsFromJSON(const json& postgresJSON) const {
    return { postgresJSON["host"],
             postgresJSON["port"],
             "devola",
             "devola_app",
             env(std::format("{}/settings/.env", ROOT_DIRECTORY))["POSTGRES_APP_USER_PASSWORD"] };
}

database_n::Result PostgreSQL::Read(const Query& query, Transaction* transaction) const {
    return {};
}

database_n::Result PostgreSQL::success(const pqxx::result& dbResult) const {
    auto result = Result().setAffected(dbResult.affected_rows());

    if (dbResult.size() <= 0)
        return result.build();

    std::vector<record_t> records;
    for (const pqxx::row& row : dbResult) {
        record_t record;

        for (const pqxx::field& field : row)
            record[field.name()] = convertField(field);

        records.emplace_back(record);
    }

    return result.setRecords(records)
                 .setStatus(Status_en::OK).build();
}

database_n::Result PostgreSQL::Update(const Query& query, Transaction* transaction) {
    return {};
}

bool PostgreSQL::validateCardinality(const pqxx::result& result, Cardinality_en expected) const {
    pqxx::result::size_type size = result.size();

    bool noneCardinalityViolated = expected == Cardinality_en::NONE and size != 0;
    bool singleCardinalityViolated = expected == Cardinality_en::SINGLE and size != 1;

    return not noneCardinalityViolated and not singleCardinalityViolated;
}