#pragma once

#include <core/utility/interface/builder.hpp>
#include <database/value.hpp>
#include <optional>


namespace database_n
{
    class Query : public Builder_i<Query> {
    public:
        using operator_t = std::string;
        using filter_t = std::unordered_map<std::string, std::pair<operator_t, Value>>;
        enum class Type_en : uint8_t {
            TARGETED,
            PROCEDURE
        };

        enum class Cardinality_en : uint8_t{
            NONE = 0,
            SINGLE = 1,
            MULTIPLE = 2
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
        const std::optional<Options>& options() const;
        const std::optional<std::vector<std::string>>& projection() const;
        const std::string& target() const;
        Type_en type() const;

        Query& setCardinality(Cardinality_en cardinality);
        Query& setData(record_t data);
        Query& setFilter(filter_t filter);
        Query& setOptions(Options options);
        Query& setProjection(std::vector<std::string> projection);
        Query& setTarget(std::string_view target);
        Query& setType(Type_en type);

    private:
        std::string m_target;
        Type_en m_type;
        Cardinality_en m_cardinality;

        std::optional<record_t> m_data;
        std::optional<filter_t> m_filter;
        std::optional<Options> m_options;
        std::optional<std::vector<std::string>> m_projection;
    };
}