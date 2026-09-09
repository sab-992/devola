#include <database/redis.hpp>

// ----------------------------------------------------------
//                          Command
// ----------------------------------------------------------

Redis::Command::Command(const PrivateCommand_s&, redisContext* context, std::string_view command) {
    m_reply = static_cast<redisReply*>(redisCommand(context, command.data()));
    if (m_reply == nullptr)
        throw DatabaseException(std::format("Unexpected reply or connection issue. Command: \"{}\"", command));

    if (m_reply->type == REDIS_REPLY_ERROR)
        throw DatabaseException(std::format("Error in the reply. Command: \"{}\"", command));
}

Redis::Command::~Command() {
    if (m_reply)
        freeReplyObject(m_reply);
}

redisReply* Redis::Command::reply() {
    return m_reply;
}

std::unique_ptr<Redis::Command> Redis::Command::create(redisContext* context, std::string_view command) {
    return std::make_unique<Redis::Command>(PrivateCommand_s(), context, command);
}

// ----------------------------------------------------------
//                           Redis
// ----------------------------------------------------------

Redis::Redis(const Private_s&, const json& redisJSON) {
    // TODO: add in README to create two redis-cli/valkey services -> user on port 6380 and rss on 6390
    m_options = optionsFromJSON(redisJSON);
    m_pool = std::make_unique<ConnectionPool>(m_options);
}

database_n::Result Redis::Create(const Query& query, Transaction* Transaction) {
    try {
        const auto& access = m_pool->access();

        validateQuery(query);

        if (not query.data().has_value())
            throw DatabaseException("Create query need to have a value for data() field.");

        const Value data = query.data().value().at(query.target());
        const auto& command = Command::create(access->connection(), std::format("SET {} {}{} NX", query.target().c_str(), fromValue(data), extractTTL(query)));

        return buildResult(command->reply(), query);
    } catch (const DatabaseException& e) {
        throw;
    } catch (const std::exception& e) {
        m_light->log(log_n::Level_en::ERROR, EXTRA_LOGS, "Error while reading from database:", e.what());
        return Result().setError(e.what()).build();
    }
}

database_n::Result Redis::Read(const Query& query, Transaction* transaction) {
    try {
        const auto& access = m_pool->access();

        validateQuery(query);

        const auto& command = Command::create(access->connection(), std::format("GET {}", query.target().c_str()));
        return buildResult(command->reply(), query);
    } catch (const DatabaseException& e) {
        throw;
    } catch (const std::exception& e) {
        m_light->log(log_n::Level_en::ERROR, EXTRA_LOGS, "Error while reading from database:", e.what());
        return Result().setError(e.what()).build();
    }
}

database_n::Result Redis::Update(const Query& query, Transaction* transaction) {
    try {
        const auto& access = m_pool->access();

        validateQuery(query);

        if (not query.data().has_value())
            throw DatabaseException("Create query need to have a value for data() field.");

        const Value data = query.data().value().at(query.target());
        const auto& command = Command::create(access->connection(), std::format("SET {} {}{} XX", query.target().c_str(), fromValue(data), extractTTL(query)));

        return buildResult(command->reply(), query);
    } catch (const DatabaseException& e) {
        throw;
    } catch (const std::exception& e) {
        m_light->log(log_n::Level_en::ERROR, EXTRA_LOGS, "Error while reading from database:", e.what());
        return Result().setError(e.what()).build();
    }
}

database_n::Result Redis::Delete(const Query& query, Transaction* transaction) {
    try {
        const auto& access = m_pool->access();

        validateQuery(query);

        if (query.cardinality() == Cardinality_en::SINGLE)
            throw DatabaseException("Redis DELETE queries cardinality can only be SINGLE");

        const auto& command = Command::create(access->connection(), std::format("DEL {}", query.target().c_str()));
        redisReply* reply = command->reply();

        if (not reply)
            throw DatabaseException("Unexpected nullptr, reply should contain a value");

        return Result().setStatus(Status_en::OK)
                       .setAffected(fromRedisReply(reply).asInt64()).build();
    } catch (const DatabaseException& e) {
        throw;
    } catch (const std::exception& e) {
        m_light->log(log_n::Level_en::ERROR, EXTRA_LOGS, "Error while reading from database:", e.what());
        return Result().setError(e.what()).build();
    }
}

database_n::Result Redis::Other(const Query& query, Transaction* transaction) {
    throw NotSupported("Other method is not supported for the Redis database");
}

database_n::Result Redis::buildResult(redisReply* dbResult, const Query& query) const {
    if (not dbResult)
        throw DatabaseException("Unexpected nullptr, dbResult should contain a value");

    Result result = Result().setStatus(Status_en::OK).setAffected(1);

    if (query.cardinality() == Cardinality_en::NONE and dbResult->type == REDIS_REPLY_STATUS)
        return result.build();

    const database_n::Value& val = fromRedisReply(dbResult);
    if (not val.isNull())
        return result.setRecords(std::vector<record_t>{{{ query.target(), val }}}).build();

    return result.build();
}

std::string Redis::extractTTL(const Query& query) const {
    if (not query.options().has_value() or not query.options()->ttl.has_value())
        return "";

    return std::format("{}", query.options()->ttl.value());
}

database_n::Value Redis::fromRedisReply(const redisReply *reply) const {
    if (not reply)
        throw DatabaseException("Unexpected nullptr, dbResult should contain a value");

    switch (reply->type) {
        case REDIS_REPLY_NIL:
            return Value();

        case REDIS_REPLY_INTEGER:
            return Value(static_cast<int64_t>(reply->integer));

        case REDIS_REPLY_DOUBLE:
            return Value(reply->dval);

        case REDIS_REPLY_BOOL:
            return Value{ static_cast<bool>(reply->integer) };

        case REDIS_REPLY_STRING:
            return Value{ std::string(reply->str, reply->len) };

        case REDIS_REPLY_ARRAY:
        case REDIS_REPLY_SET: {
            const auto& vector = std::make_shared<std::vector<Value>>();

            vector->reserve(reply->elements);
            for (size_t i = 0; i < reply->elements; ++i)
                vector->emplace_back(fromRedisReply(reply->element[i]));

            return Value(std::move(vector));
        }
        case REDIS_REPLY_MAP: {
            const auto& map = std::make_shared<record_t>();

            map->reserve(reply->elements / 2);
            for (size_t i = 0; i + 1 < reply->elements; i += 2)
                map->emplace(fromRedisReply(reply->element[i]).asString(),
                             fromRedisReply(reply->element[i + 1]));

            return Value{ std::move(map) };
        }
        default:
            throw NotSupported("Unexpected redisReply type");
    }
}

std::string Redis::fromValue(const Value& value) const {
    std::string result;

    std::visit(overloads{
        [](const std::shared_ptr<Query>&) {              throw NotSupported("Value is a nested Query. It cannot be bound as Redis params"); },
        [](const std::shared_ptr<record_t>&) {           throw NotSupported("Value is a nested record. It cannot be bound as Redis params"); },
        [](const std::monostate&) {                      throw NotSupported("Value is empty. It cannot be bound as Redis params"); },
        [](const std::vector<std::byte>&) {              throw NotSupported("Value is a nested vector of bytes. It cannot be bound as Redis params"); },
        [](const std::shared_ptr<std::vector<Value>>&) { throw NotSupported("Value is a nested vector of Values. It cannot be bound as Redis params"); },
        [&result, &value = std::as_const(value)](auto&& arg) { result = Converter::toString(arg); }
    }, value.raw());

    return result;
}

std::shared_ptr<Redis> Redis::instance(const json& redisJSON) {
    static std::shared_ptr<Redis> instance = std::make_shared<Redis>(Private_s(), redisJSON);
    return instance;
}

redis_n::Options Redis::optionsFromJSON(const json& redisJSON) const {
    return { redisJSON["host"],
             redisJSON["port"],
             env(std::format("{}/settings/.env", ROOT_DIRECTORY))["REDIS_APP_PASSWORD"],
             redisJSON["database"] };
}

void Redis::validateQuery(const Query& query) const {
    if (query.type() != database_n::Query::Type_en::TARGETED)
        throw DatabaseException("Redis queries can only be targeted");

    if (query.cardinality() == Cardinality_en::MULTIPLE)
        throw DatabaseException("Redis queries cardinality is either NONE or SINGLE");

    if (query.target().empty())
        throw DatabaseException("Redis queries need to have a non empty target");
}