#include <database/value.hpp>


database_n::Value::Value(bool boolean) : m_variant(boolean) {}
database_n::Value::Value(int64_t integer) : m_variant(integer) {}
database_n::Value::Value(int integer) : m_variant(static_cast<int64_t>(integer)) {}
database_n::Value::Value(double decimal) : m_variant(decimal) {}
database_n::Value::Value(std::string string) : m_variant(std::move(string)) {}
database_n::Value::Value(std::string_view string) : m_variant(std::string(std::move(string))) {}
database_n::Value::Value(const char* string) : m_variant(std::string(string)) {}
database_n::Value::Value(std::vector<std::byte> bytes) : m_variant(std::move(bytes)) {}
database_n::Value::Value(std::shared_ptr<Query> query) : m_variant(std::move(query)) {}
database_n::Value::Value(std::shared_ptr<record_t> record) : m_variant(std::move(record)) {}
database_n::Value::Value(std::shared_ptr<std::vector<Value>> values) : m_variant(std::move(values)) {}

bool database_n::Value::operator==(const Value& other) const {
    return m_variant == other.m_variant;
}

bool database_n::Value::operator!=(const Value& other) const {
    return !(*this == other);
}

bool database_n::Value::asBool() const {
    return std::get<bool>(m_variant);
}

const std::vector<std::byte>& database_n::Value::asBytes() const {
    return std::get<std::vector<std::byte>>(m_variant);
}

double database_n::Value::asDouble() const {
    return std::get<double>(m_variant);
}

int64_t database_n::Value::asInt64() const {
    return std::get<int64_t>(m_variant);
}

const std::shared_ptr<std::vector<database_n::Value>>& database_n::Value::asList() const {
    return std::get<std::shared_ptr<std::vector<Value>>>(m_variant);
}

const std::shared_ptr<database_n::Query>& database_n::Value::asQuery() const {
    return std::get<std::shared_ptr<Query>>(m_variant);
}

const std::shared_ptr<database_n::record_t>& database_n::Value::asRecord() const {
    return std::get<std::shared_ptr<record_t>>(m_variant);
}

const std::string& database_n::Value::asString() const {
    return std::get<std::string>(m_variant);
}

bool database_n::Value::isNull() const {
    return std::holds_alternative<std::monostate>(m_variant);
}

const database_n::Value::variant_t& database_n::Value::raw() const {
    return m_variant;
}