#include <database/query.hpp>


database_n::Query::Query() {}
database_n::Query::~Query() {}

database_n::Query& database_n::Query::build() & {
    return *this;
}

database_n::Query database_n::Query::build() && {
    return *this;
}

database_n::Query::Cardinality_en database_n::Query::cardinality() const {
    return m_cardinality;
}

std::optional<database_n::record_t> database_n::Query::data() const {
    return m_data;
}

std::optional<database_n::Query::Options> database_n::Query::extraOptions() const {
    return m_options;
}

std::optional<database_n::record_t> database_n::Query::filter() const {
    return m_filter;
}

std::optional<std::vector<std::string>> database_n::Query::projection() const {
    return m_projection;
}

database_n::Query& database_n::Query::setCardinality(Cardinality_en cardinality) {
    m_cardinality = cardinality;
    return *this;
}

database_n::Query& database_n::Query::setData(record_t data) {
    m_data = std::move(data);
    return *this;
}

database_n::Query& database_n::Query::setExtraOptions(Options options) {
    m_options = std::move(options);
    return *this;
}

database_n::Query& database_n::Query::setFilter(record_t filter) {
    m_filter = std::move(filter);
    return *this;
}

database_n::Query& database_n::Query::setProjection(std::vector<std::string> projection) {
    m_projection = std::move(projection);
    return *this;
}

database_n::Query& database_n::Query::setTarget(std::string_view target) {
    m_target = target;
    return *this;
}

database_n::Query& database_n::Query::setType(Type_en type) {
    m_type = type;
    return *this;
}

std::string database_n::Query::target() const {
    return m_target;
}

database_n::Query::Type_en database_n::Query::type() const {
    return m_type;
}