#pragma once

#include <core/utility/interface/builder.hpp>
#include <database/value.hpp>
#include <optional>


namespace database_n
{
    class Query : public Builder_i<Query> {
    public:
        enum class Type_en : uint8_t {
            TARGETED,
            PROCEDURE
        };

        enum class Cardinality_en : uint8_t{
            NONE = 0,
            SINGLE = 1,
            MULTIPLE = 2
        };

        struct SortField {
            std::string field;
            bool ascending = true;
        };

        struct Options {
            std::optional<size_t> limit;
            std::optional<size_t> offset;
            std::optional<std::vector<SortField>> sort;
        };

        Query();
        ~Query();

        Query& build() & override;
        Query build() && override;

        Cardinality_en cardinality() const;
        std::optional<record_t> data() const;
        std::optional<Options> extraOptions() const;
        std::optional<record_t> filter() const;
        std::optional<std::vector<std::string>> projection() const;
        std::string target() const;
        Type_en type() const;

        Query& setCardinality(Cardinality_en cardinality);
        Query& setData(record_t data);
        Query& setExtraOptions(Options options);
        Query& setFilter(record_t filter);
        Query& setProjection(std::vector<std::string> projection);
        Query& setTarget(std::string_view target);
        Query& setType(Type_en type);

    private:
        std::string m_target;
        Type_en m_type;
        Cardinality_en m_cardinality;

        std::optional<record_t> m_data;
        std::optional<record_t> m_filter;
        std::optional<std::vector<std::string>> m_projection;
        std::optional<Options> m_options;
    };
}