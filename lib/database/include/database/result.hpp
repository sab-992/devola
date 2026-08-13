#pragma once

#include <core/network.hpp>
#include <core/utility.hpp>
#include <database/value.hpp>
#include <optional>
#include <string>
#include <vector>

namespace database_n
{
    enum class Status_en {
        OK,
        NOT_FOUND,
        CONNECTION_ERROR,
        QUERY_ERROR,
        CARDINALITY_ERROR,
        UNSUPPORTED
    };

    class Result : public Builder_i<Result> {
    public:
        Result();
        ~Result();

        Result& build() & override;
        Result build() && override;

        size_t affected() const;
        std::optional<std::string> error() const;
        bool isEmpty() const;
        bool isOK() const;
        std::optional<std::vector<record_t>> records() const;
        Status_en status() const;
        size_t size() const;

        Result& setAffected(size_t affected);
        Result& setError(std::string message);
        Result& setRecords(std::vector<record_t> records);
        Result& setStatus(Status_en status);

    private:
        size_t m_affected = 0;
        std::optional<std::string> m_error;
        std::optional<std::vector<record_t>> m_records;
        Status_en m_status;
    };
}