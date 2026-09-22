#pragma once

#include <core/exception.hpp>
#include <core/file.hpp>
#include <core/logging.hpp>
#include <core/utility.hpp>
#include <database/interface/database.hpp>
#include <database/detail/redis/options.hpp>
#include <database/detail/redis/pool.hpp>
#include <memory>


class Redis : public database_n::Database_i {
    using ConnectionPool = redis_n::ConnectionPool;
    using Cardinality_en = database_n::Query::Cardinality_en;
    using json = nlohmann::json;
    using Options = redis_n::Options;
    using Query = database_n::Query;
    using record_t = database_n::record_t;
    using Result = database_n::Result;
    using Status_en = database_n::Status_en;
    using Value = database_n::Value;

    template<class... Ts>
    struct overloads : Ts... { using Ts::operator()...; };
    struct Private_s {};

    const std::vector<std::string> EXTRA_LOGS = { "REDS" };

    class Command {
        struct PrivateCommand_s {};

    public:
        Command(const PrivateCommand_s&, redisContext* context, std::vector<std::string>&& filteredArgs);
        ~Command();

        redisReply* reply();

        template<std::convertible_to<std::string_view>... Args>
        inline static std::unique_ptr<Command> create(redisContext* context,  Args&&... args) {
            std::vector<std::string> filteredArgs;
            // TODO: also create the argvlen here and pass it as argument in constructor of Command
            ((addArgument(filteredArgs, args)), ...);
            return std::make_unique<Redis::Command>(PrivateCommand_s(), context, std::move(filteredArgs));
        }

    private:
        redisReply* m_reply;

        static void addArgument(std::vector<std::string>& vector, std::string_view arg);
    };

public:
    Redis(const Private_s&, const json& redisJSON);

    Redis(const Redis&) = delete;
    Redis& operator=(const Redis&) = delete;

    Redis(Redis&&) = delete;
    Redis& operator=(Redis&&) = delete;

    ~Redis() = default;

    Result Create(const Query& query, Transaction* transaction=nullptr) override;
    Result Read(const Query& query, Transaction* transaction=nullptr) override;
    Result Update(const Query& query, Transaction* transaction=nullptr) override;
    Result Delete(const Query& query, Transaction* transaction=nullptr) override;
    Result Other(const Query& query, Transaction* transaction=nullptr) override;

    static std::shared_ptr<Redis> instance(const json& redisJSON);

private:
    std::shared_ptr<log_n::Light> m_light;
    Options m_options;
    std::unique_ptr<redis_n::ConnectionPool> m_pool;

    Result buildResult(redisReply* dbResult, const Query& query) const;
    std::pair<std::string, std::string> extractTTL(const Query& query) const;
    Value fromRedisReply(const redisReply* reply) const;
    std::string fromValue(const Value& value) const;
    redis_n::Options optionsFromJSON(const json& redisJSON) const;
    void validateQuery(const Query& query) const;
};