#pragma once

#include <core/utility/interface/builder.hpp>
#include <database/value.hpp>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace database_n
{
    class Value;
    class Query : public Builder_i<Query> {
    public:
        using operator_t = std::string;
        using filter_t = std::unordered_map<std::string, std::pair<operator_t, Value>>;

        enum class Type_en : uint8_t {
            TARGETED,
            FUNCTION,
            INNER_QUERY,
            PROCEDURE
        };

        enum class Cardinality_en : uint8_t {
            NONE = 0,
            SINGLE = 1,
            MULTIPLE = 2
        };

        enum class JoinType_en : uint8_t {
            INNER,
            LEFT,
            RIGHT,
            FULL,
            CROSS
        };

        struct JoinCondition {
            std::string left_field;
            operator_t  op = "=";
            std::string right_field;
        };

        struct Join {
            JoinType_en type = JoinType_en::INNER;
            std::optional<std::string> target;
            std::optional<std::shared_ptr<Query>> source;
            std::optional<std::string> alias;
            std::vector<JoinCondition> on;
        };

        struct Sort {
            std::string field;
            bool ascending = true;
        };

        struct Options {
            std::optional<std::vector<std::string>> group;
            std::optional<filter_t> having;
            std::optional<size_t> limit;
            std::optional<size_t> offset;
            std::optional<std::vector<Sort>> sort;
        };

        Query();
        ~Query();

        Query& build() & override;
        Query build() && override;

        Cardinality_en cardinality() const;
        const std::optional<record_t>& data() const;
        const std::optional<filter_t>& filter() const;
        const std::optional<std::vector<Value>>& functionData() const;
        const std::optional<Options>& options() const;
        const std::optional<std::vector<std::string>>& projection() const;
        const std::string& target() const;
        Type_en type() const;

        Query& setCardinality(Cardinality_en cardinality);
        Query& setData(record_t data);
        Query& setFilter(filter_t filter);
        Query& setFunctionData(std::vector<Value> data);
        Query& setOptions(Options options);
        Query& setProjection(std::vector<std::string> projection);
        Query& setTarget(std::string_view target);
        Query& setType(Type_en type);

        const std::optional<std::string>& alias() const;
        Query& setAlias(std::string alias);

        const std::optional<std::vector<Join>>& joins() const;
        Query& setJoins(std::vector<Join> joins);
        Query& addJoin(Join join);

        const std::optional<std::pair<std::shared_ptr<Query>, std::string>>& source() const;
        Query& setSource(Query subquery, std::string alias);

    private:
        std::string m_target;
        Type_en m_type;
        Cardinality_en m_cardinality;

        std::optional<std::string> m_alias;
        std::optional<record_t> m_data;
        std::optional<filter_t> m_filter;
        std::optional<std::vector<Join>> m_joins;
        std::optional<Options> m_options;
        std::optional<std::vector<std::string>> m_projection;
        std::optional<std::pair<std::shared_ptr<Query>, std::string>> m_source;
        std::optional<std::vector<database_n::Value>> m_functionData;
    };
}