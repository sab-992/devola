#pragma once

#include <cstdint>
#include <cstddef>
#include <database/detail/types.hpp>
#include <memory>
#include <string>
#include <variant>
#include <vector>


namespace database_n
{
    class Query;
    class Value {
    public:
        using variant_t = std::variant<std::monostate,
                                    bool,
                                    int64_t,
                                    double,
                                    std::string,
                                    std::vector<std::byte>,
                                    std::shared_ptr<Query>,
                                    std::shared_ptr<record_t>,
                                    std::shared_ptr<std::vector<Value>>>;

        Value() = default;
        Value(bool boolean);
        Value(int64_t integer);
        Value(int integer);
        Value(double decimal);
        Value(std::string string);
        Value(std::string_view string);
        Value(const char* string);
        Value(std::vector<std::byte> bytes);
        Value(std::shared_ptr<Query> query);
        Value(std::shared_ptr<record_t> record);
        Value(std::shared_ptr<std::vector<Value>> values);
        ~Value() = default;

        bool asBool() const;
        const std::vector<std::byte>& asBytes() const;
        double asDouble() const;
        int64_t asInt64() const;
        const std::string& asString() const;
        const std::shared_ptr<Query>& asQuery() const;
        const std::shared_ptr<record_t>& asRecord() const;
        const std::shared_ptr<std::vector<Value>>& asList() const;

        bool isNull() const;

        template<typename T>
        T get() const { return std::get<T>(m_variant); }

        template<typename T>
        bool holds() const { return std::holds_alternative<T>(m_variant); }

        const variant_t& raw() const;

        bool operator==(const Value& other) const;
        bool operator!=(const Value& other) const;

    private:
        variant_t m_variant;
    };
}