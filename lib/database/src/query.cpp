#include <database/query.hpp>


database_n::Query::Query() : m_cardinality(Cardinality_en::NONE) {}
database_n::Query::~Query() {}

database_n::Query& database_n::Query::addJoin(Join join) {
    if (not m_joins)
        m_joins = std::vector<Join>{};

    m_joins->push_back(std::move(join));
    return *this;
}

const std::optional<std::string>& database_n::Query::alias() const {
    return m_alias;
}

database_n::Query& database_n::Query::build() & {
    if (m_type == Type_en::INNER_QUERY)
        m_target = "query";

    return *this;
}

database_n::Query database_n::Query::build() && {
    if (m_type == Type_en::INNER_QUERY)
        m_target = "query";

    return *this;
}

database_n::Query::Cardinality_en database_n::Query::cardinality() const {
    return m_cardinality;
}

const std::optional<database_n::record_t>& database_n::Query::data() const {
    return m_data;
}

const std::optional<database_n::Query::filter_t>& database_n::Query::filter() const {
    return m_filter;
}

const std::optional<std::vector<database_n::Value>>& database_n::Query::functionData() const {
    return m_functionData;
}

const std::optional<std::vector<database_n::Query::Join>>& database_n::Query::joins() const {
    return m_joins;
}

const std::optional<database_n::Query::Options>& database_n::Query::options() const {
    return m_options;
}

const std::optional<std::vector<std::string>>& database_n::Query::projection() const {
    return m_projection;
}

const std::optional<std::pair<std::shared_ptr<database_n::Query>, std::string>>& database_n::Query::source() const {
    return m_source;
}

database_n::Query& database_n::Query::setAlias(std::string alias) {
    m_alias = std::move(alias);
    return *this;
}

database_n::Query& database_n::Query::setCardinality(Cardinality_en cardinality) {
    m_cardinality = cardinality;
    return *this;
}

database_n::Query& database_n::Query::setData(record_t data) {
    m_data = std::move(data);
    return *this;
}

database_n::Query& database_n::Query::setFilter(filter_t filter) {
    m_filter = std::move(filter);
    return *this;
}

database_n::Query& database_n::Query::setFunctionData(std::vector<Value> data) {
    m_functionData = std::move(data);
    return *this;
}

database_n::Query& database_n::Query::setJoins(std::vector<Join> joins) {
    m_joins = std::move(joins);
    return *this;
}

database_n::Query& database_n::Query::setOptions(Options options) {
    m_options = std::move(options);
    return *this;
}

database_n::Query& database_n::Query::setProjection(std::vector<std::string> projection) {
    m_projection = std::move(projection);
    return *this;
}

database_n::Query& database_n::Query::setSource(Query subquery, std::string alias) {
    subquery.setType(Type_en::INNER_QUERY);
    m_source = std::make_pair(std::make_shared<Query>(std::move(subquery)), std::move(alias));
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

const std::string& database_n::Query::target() const {
    return m_target;
}

database_n::Query::Type_en database_n::Query::type() const {
    return m_type;
}