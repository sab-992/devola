#include <database/result.hpp>


database_n::Result::Result() {}
database_n::Result::~Result() {}

database_n::Result& database_n::Result::build() & {
    return *this;
}

database_n::Result database_n::Result::build() && {
    return *this;
}

size_t database_n::Result::affected() const {
    return m_affected;
}

std::optional<std::string> database_n::Result::error() const {
    return m_error;
}

bool database_n::Result::isEmpty() const {
    return not m_records.has_value() or m_records->empty();
}

bool database_n::Result::isOK() const {
    return m_status == Status_en::OK and not m_error.has_value();
}

std::optional<std::vector<database_n::record_t>> database_n::Result::records() const {
    return m_records;
}

database_n::Result& database_n::Result::setAffected(size_t affected) {
    m_affected = affected;
    return *this;
}

database_n::Result& database_n::Result::setError(std::string message) {
    m_error = message;
    return *this;
}

database_n::Result& database_n::Result::setRecords(std::vector<record_t> records) {
    m_records = records;
    return *this;
}

database_n::Result& database_n::Result::setStatus(Status_en status) {
    m_status = status;
    return *this;
}

size_t database_n::Result::size() const {
    return m_records->size();
}