#include <database/postgres.hpp>


PostgreSQL::PostgreSQL(const Private_s&, const json& postgresJSON) : m_light(log_n::Light::instance()) {
    m_options = optionsFromJSON(postgresJSON);
    m_pool = std::make_unique<ConnectionPool>(m_options);
};

void PostgreSQL::addParam(pqxx::params& params, const Value& value) const {
    std::visit(overloads{
        [&](const std::shared_ptr<Query>&) {},
        [&](const std::shared_ptr<record_t>&) {           throw NotSupported("Value is a nested record. It cannot be bound as Postgres params"); },
        [&](const std::shared_ptr<std::vector<Value>>&) { throw NotSupported("Value is a nested list of values. It cannot be bound as Postgres params"); },
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
            result += " AND ";

        const std::string& op = rvalue.first;
        const Value& value = rvalue.second;

        if (value.holds<std::shared_ptr<Query>>()) {
            result += std::format("{} {} ({})", column, op, selectSql(*value.asQuery(), params, placeholders));
            first = false;
            continue;
        }

        if (value.isNull()) {
            result += std::format("{} {}", column, op);
            first = false;
            continue;
        }

        std::string placeholder = placeholders.get();
        placeholders.next();
        addParam(params, value);

        if (op == "IN")
            result += std::format("{} = ANY({})", column, placeholder);
        else if (op == "NOT IN")
            result += std::format("{} <> ALL({})", column, placeholder);
        else
            result += std::format("{} {} {}", column, op, placeholder);

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

    std::unique_ptr<Transaction> ownedTransaction;
    pgsql_n::Transaction& tx = beginTransaction(transaction, ownedTransaction);
    try {
        pqxx::params params;
        pqxx::placeholders<> placeholders;

        std::string sql = std::format("DELETE FROM {}", query.target());
        sql += where(query, params, placeholders);

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
        m_light->log(log_n::Level_en::ERROR, EXTRA_LOGS, "ABORTED: error during deletion: ", e.what());
        return Result().setError(e.what())
                       .setStatus(Status_en::QUERY_ERROR).build();
    }
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

std::string PostgreSQL::from(const Query& query, pqxx::params& params, pqxx::placeholders<>& placeholders) const {
    if (query.source().has_value()) {
        const auto& [subquery, alias] = query.source().value();
        return std::format("({}) AS {}", selectSql(*subquery, params, placeholders), alias);
    }

    if (query.type() != Query::Type_en::FUNCTION)
        return query.target();

    if (not query.functionData().has_value())
        throw LogicException("FUNCTION query requires function data to pass in parameter.");

    std::string clause = std::format("{}(", query.target());
    for (const auto& elem : query.functionData().value()) {
        clause += placeholders.get();
        addParam(params, elem);
        placeholders.next();
    }
    return clause + ")";
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

    return std::format(" HAVING {}", condition(having, params, placeholders));
}

std::string PostgreSQL::join(const Query& query, pqxx::params& params, pqxx::placeholders<>& placeholders) const {
    if (not query.joins().has_value())
        return "";

    std::string result;
    for (const auto& j : query.joins().value()) {
        std::string joinTarget = j.source.has_value() ? std::format("({}) AS {}", selectSql(*j.source.value(), params, placeholders), j.alias.value()) : j.alias.has_value() ? std::format("{} {}", j.target.value(), j.alias.value()) : j.target.value();
        result += std::format(" {} JOIN {}", joinTypeToStr(j.type), joinTarget);
        if (j.type != Query::JoinType_en::CROSS and not j.on.empty())
            result += std::format(" ON {}", onCondition(j.on));
    }
    return result;
}

std::string PostgreSQL::joinTypeToStr(Query::JoinType_en type) const {
    switch (type) {
        case Query::JoinType_en::INNER: return "INNER";
        case Query::JoinType_en::LEFT:  return "LEFT";
        case Query::JoinType_en::RIGHT: return "RIGHT";
        case Query::JoinType_en::FULL:  return "FULL";
        case Query::JoinType_en::CROSS: return "CROSS";
    }
    throw LogicException("Unhandled Query::JoinType_en");
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

std::string PostgreSQL::onCondition(const std::vector<Query::JoinCondition>& conditions) const {
    std::string result;
    for (size_t i = 0; i < conditions.size(); i++) {
        if (i != 0)
            result += " AND ";

        result += std::format("{} {} {}", conditions[i].left_field, conditions[i].op, conditions[i].right_field);
    }
    return result;
}

pgsql_n::Options PostgreSQL::optionsFromJSON(const json& postgresJSON) const {
    return { postgresJSON["host"],
             postgresJSON["port"],
             "devola",
             "devola_app",
             env(std::format("{}/settings/.env", ROOT_DIRECTORY))["POSTGRES_APP_PASSWORD"] };
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
        if (query.functionData().has_value()) {
            const auto& functionData = query.functionData().value();
            for (size_t i = 0; i < functionData.size(); i++) {
                addParam(params, functionData[i]);
                values.append(i != 0 ? std::format(", ${}", i + 1) : std::format("${}", i + 1));
            }
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
        m_light->log(log_n::Level_en::ERROR, EXTRA_LOGS, "ABORTED: error during procedure: ", e.what());
        return Result().setError(e.what())
                       .setStatus(Status_en::QUERY_ERROR).build();
    }
}

std::string PostgreSQL::projection(const Query& query) const {
    if (not query.projection().has_value())
        return "*";

    const std::vector<std::string>& columns = query.projection().value();

    std::string result;
    for (size_t i = 0; i < columns.size(); i++)
        result.append(i != 0 ? std::format(", {}", columns[i]) : columns[i]);

    return result;
}

database_n::Result PostgreSQL::Read(const Query& query, Transaction* transaction) {
    if (query.type() != Query::Type_en::TARGETED and query.type() != Query::Type_en::FUNCTION)
        throw LogicException(std::format("{} expects TARGETED or FUNCTION query:", FUNCTION_SIGNATURE));

    std::unique_ptr<Transaction> ownedTransaction;
    pgsql_n::Transaction& tx = beginTransaction(transaction, ownedTransaction);
    try {
        pqxx::params params;
        pqxx::placeholders<> placeholders;
        pqxx::result queryResult = tx.execute(std::format("{};", selectSql(query, params, placeholders)), params);

        if (not validateCardinality(queryResult, query.cardinality()))
            return Result().setError(std::format("Expected ({} rows) but got: ({} rows)", to_underlying(query.cardinality()), queryResult.size()))
                           .setStatus(Status_en::CARDINALITY_ERROR).build();

        if (ownedTransaction)
            tx.commit();

        return success(queryResult);
    } catch (std::exception& e) {
        tx.abort();
        m_light->log(log_n::Level_en::ERROR, EXTRA_LOGS, "ABORTED: error during read: ", e.what());
        return Result().setError(e.what())
                       .setStatus(Status_en::QUERY_ERROR).build();
    }
}

std::string PostgreSQL::selectSql(const Query& query, pqxx::params& params, pqxx::placeholders<>& placeholders) const {
    std::string sql = std::format("SELECT {} FROM {}", projection(query), from(query, params, placeholders));
    sql += join(query, params, placeholders);
    sql += where(query, params, placeholders);
    sql += groupBy(query);
    sql += having(query, params, placeholders);
    sql += orderBy(query);
    sql += limit(query);
    sql += offset(query);
    return sql;
}

database_n::Result PostgreSQL::success(const pqxx::result& dbResult) const {
    auto result = Result().setStatus(Status_en::OK);

    if(dbResult.empty())
        return result.setAffected(0).build();

    try {
        result.setAffected(dbResult.affected_rows());
    } catch (const std::exception&) {
        result.setAffected(dbResult.size());
    }

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

    std::unique_ptr<Transaction> ownedTransaction;
    pgsql_n::Transaction& tx = beginTransaction(transaction, ownedTransaction);
    try {
        if (not query.data().has_value())
            return Result().setError("No data to update")
                           .setStatus(Status_en::QUERY_ERROR).build();

        pqxx::params params;
        pqxx::placeholders<> placeholders;
        std::string setClause;

        bool first = true;
        for (const auto& [column, value] : query.data().value()) {
            if (not first)
                setClause += ", ";
            std::string placeholder = placeholders.get();
            placeholders.next();
            addParam(params, value);
            setClause += std::format("{} = {}", column, placeholder);
            first = false;
        }

        std::string sql = std::format("UPDATE {} SET {}", query.target(), setClause);
        sql += where(query, params, placeholders);

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
        m_light->log(log_n::Level_en::ERROR, EXTRA_LOGS, "ABORTED: error during update: ", e.what());
        return Result().setError(e.what())
                       .setStatus(Status_en::QUERY_ERROR).build();
    }
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