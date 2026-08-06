#include <database/postgre.hpp>


PostgreSQL::PostgreSQL(const Private_s&, const json& postgresJSON) : m_light(log_n::Light::instance()) {
    m_options = optionsFromJSON(postgresJSON);
    m_pool = std::make_unique<ConnectionPool>(m_options);
};

void PostgreSQL::addParam(pqxx::params& params, const Value& value) const {
    std::visit(overloads{
        [&](const std::shared_ptr<record_t>&) { throw LogicException("Value is a nested record. It cannot be bound as Postgres params"); },
        [&](const std::shared_ptr<std::vector<Value>>&) { throw LogicException("Value is a nested list of values. It cannot be bound as Postgres params"); },
        [&, &value = std::as_const(value)](auto&& arg) {
            if (value.isNull())
                params.append();
            else
                params.append(arg);
        }
    }, value.raw());
}

pgsql_n::Transaction& PostgreSQL::beginTransaction(Transaction*& transaction, std::unique_ptr<Transaction>& ownedTransaction) {
    if (not transaction) {
        ownedTransaction = std::make_unique<Transaction>(pgsql_n::Transaction());
        transaction = ownedTransaction.get();
    }

    auto& tx = transaction->get<pgsql_n::Transaction>();
    tx.begin(*m_pool);

    return tx;
}

std::string PostgreSQL::condition(const Query::filter_t& filter, pqxx::params& params, pqxx::placeholders<>& placeholders) const {
    std::string result;
    bool first = true;
    for (const auto& [column, rvalue] : filter) {
        if (not first)
            result += "AND ";

        result += std::format("{} {}", column, rvalue.first);
        if (not rvalue.second.isNull()) {
            result += std::format(" {}", placeholders.get());
            placeholders.next();
            addParam(params, rvalue.second);
        }

        first = false;
    }
    return result;
}

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
    if (query.type() != Query::Type_en::TARGETED)
        throw LogicException(std::format("{} expects TARGETED query:", FUNCTION_SIGNATURE));

    std::unique_ptr<Transaction> ownedTransaction;
    pgsql_n::Transaction& tx = beginTransaction(transaction, ownedTransaction);
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
        m_light->log(log_n::Level_en::ERROR, EXTRA_LOGS, "ABORTED: error during insertion: ", e.what());
        return Result().setError(e.what())
                       .setStatus(Status_en::QUERY_ERROR).build();
    }
}

database_n::Result PostgreSQL::Delete(const Query& query, Transaction* transaction) {
    if (query.type() != Query::Type_en::TARGETED)
        throw LogicException(std::format("{} expects TARGETED query:", FUNCTION_SIGNATURE));

    return {};
}

std::pair<std::vector<std::string>, pqxx::params> PostgreSQL::extractParams(const record_t& record) const {
    std::vector<std::string> columns;
    pqxx::params params;

    for (const auto& [column, value] : record) {
        columns.emplace_back(column);
        addParam(params, value);
    }

    return { columns, params };
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

std::string PostgreSQL::groupBy(const Query& query) const {
    if (not query.options().has_value() or
        not query.options().value().group.has_value())
        return "";

    const auto& columns = query.options().value().group.value();
    if (columns.empty())
        return "";

    std::string result = " GROUP BY ";
    for (size_t i = 0; i < columns.size(); i++) {
        if (i != 0)
            result += ", ";

        result += columns[i];
    }
    return result;
}

std::string PostgreSQL::having(const Query& query, pqxx::params& params, pqxx::placeholders<>& placeholders) const {
    if (not query.options().has_value() or not query.options().value().having.has_value())
        return "";

    const auto& having = query.options().value().having.value();
    if (having.empty())
        return "";

    return std::format(" HAVING {}", condition(query.filter().value(), params, placeholders));
}

std::string PostgreSQL::limit(const Query& query) const {
    if (not query.options().has_value() or not query.options().value().limit.has_value())
        return "";

    return std::format(" LIMIT {}", query.options().value().limit.value());
}

std::string PostgreSQL::offset(const Query& query) const {
    if (not query.options().has_value() or not query.options().value().offset.has_value())
        return "";

    return std::format(" OFFSET {}", query.options().value().offset.value());
}

pgsql_n::Options PostgreSQL::optionsFromJSON(const json& postgresJSON) const {
    return { postgresJSON["host"],
             postgresJSON["port"],
             "devola",
             "devola_app",
             env(std::format("{}/settings/.env", ROOT_DIRECTORY))["POSTGRES_APP_USER_PASSWORD"] };
}

std::string PostgreSQL::orderBy(const Query& query) const {
    if (not query.options().has_value() or not query.options().value().sort.has_value())
        return "";

    const auto& sort = query.options().value().sort.value();
    if (sort.empty())
        return "";

    std::string result = " ORDER BY ";
    for (size_t i = 0; i < sort.size(); i++) {
        if (i != 0)
            result += ", ";

        result += std::format("{} {}", sort[i].field, sort[i].ascending ? "ASC" : "DESC");
    }
    return result;
}

database_n::Result PostgreSQL::Other(const Query& query, Transaction* transaction) {
    if (query.type() != Query::Type_en::PROCEDURE)
        throw LogicException(std::format("{} expects PROCEDURE query:", FUNCTION_SIGNATURE));

    std::unique_ptr<Transaction> ownedTransaction;
    pgsql_n::Transaction& tx = beginTransaction(transaction, ownedTransaction);
    try {
        std::string values;
        pqxx::params params;
        if (query.data().has_value()) {
            params = extractParams(query.data().value()).second;

            for (size_t i = 0; i < params.size(); i++)
                values.append(i != 0 ? ", " : std::format("${}", i + 1));
        }

        pqxx::result queryResult = tx.execute(std::format("CALL {}({})", query.target(), values), params);

        if (not validateCardinality(queryResult, query.cardinality()))
            return Result().setError(std::format("Expected ({} rows) but got: ({} rows)", to_underlying(query.cardinality()), queryResult.size()))
                           .setStatus(Status_en::CARDINALITY_ERROR).build();

        if (ownedTransaction)
            tx.commit();

        return success(queryResult);
    } catch (std::exception& e) {
        tx.abort();
        m_light->log(log_n::Level_en::ERROR, EXTRA_LOGS, "ABORTED: error during insertion: ", e.what());
        return Result().setError(e.what())
                       .setStatus(Status_en::QUERY_ERROR).build();
    }
}

database_n::Result PostgreSQL::Read(const Query& query, Transaction* transaction) {
    if (query.type() != Query::Type_en::TARGETED)
        throw LogicException(std::format("{} expects TARGETED query:", FUNCTION_SIGNATURE));

    std::unique_ptr<Transaction> ownedTransaction;
    pgsql_n::Transaction& tx = beginTransaction(transaction, ownedTransaction);
    try {
        std::string projection;
        if (not query.projection().has_value())
            projection = "*";
        else {
            std::vector<std::string> columns = query.projection().value();
            for (size_t i = 0; i < columns.size(); i++)
                projection.append(i != 0 ? std::format(", {}", columns[i]) : columns[i]);
        }

        pqxx::params params;
        pqxx::placeholders placeholders;
        std::string sql = std::format("SELECT {} FROM {}{}{}{}{}{};", projection, query.target(), where(query, params, placeholders),
                                                                                                  groupBy(query),
                                                                                                  having(query, params, placeholders),
                                                                                                  orderBy(query),
                                                                                                  limit(query),
                                                                                                  offset(query));

        m_light->log(log_n::Level_en::SPECIAL, EXTRA_LOGS, "SQL READ CMD:", sql);

        pqxx::result queryResult = tx.execute(sql, params);

        if (not validateCardinality(queryResult, query.cardinality()))
            return Result().setError(std::format("Expected ({} rows) but got: ({} rows)", to_underlying(query.cardinality()), queryResult.size()))
                           .setStatus(Status_en::CARDINALITY_ERROR).build();

        if (ownedTransaction)
            tx.commit();

        return success(queryResult);
    } catch (std::exception& e) {
        tx.abort();
        m_light->log(log_n::Level_en::ERROR, EXTRA_LOGS, "ABORTED: error during insertion: ", e.what());
        return Result().setError(e.what())
                       .setStatus(Status_en::QUERY_ERROR).build();
    }

    return {};
}

database_n::Result PostgreSQL::success(const pqxx::result& dbResult) const {
    auto result = Result().setStatus(Status_en::OK);

    if(dbResult.empty())
        return result.setAffected(0).build();

    result.setAffected(dbResult.affected_rows());

    std::vector<record_t> records;
    for (const pqxx::row& row : dbResult) {
        record_t record;

        for (const pqxx::field& field : row)
            record[field.name()] = convertField(field);

        records.emplace_back(record);
    }

    return result.setRecords(records).build();
}

database_n::Result PostgreSQL::Update(const Query& query, Transaction* transaction) {
    if (query.type() != Query::Type_en::TARGETED)
        throw LogicException(std::format("{} expects TARGETED query:", FUNCTION_SIGNATURE));

    return {};
}

bool PostgreSQL::validateCardinality(const pqxx::result& result, Cardinality_en expected) const {
    pqxx::result::size_type size = result.size();

    bool noneCardinalityViolated = expected == Cardinality_en::NONE and size != 0;
    bool singleCardinalityViolated = expected == Cardinality_en::SINGLE and size != 1;

    return not noneCardinalityViolated and not singleCardinalityViolated;
}

std::string PostgreSQL::where(const Query& query, pqxx::params& params, pqxx::placeholders<>& placeholders) const {
    if (not query.filter().has_value())
        return "";

    return std::format(" WHERE {}", condition(query.filter().value(), params, placeholders));
}