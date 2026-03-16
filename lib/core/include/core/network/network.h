#pragma once

#include <core/conversion/string_convertible.h>
#include <format>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>


namespace network_n
{
    namespace protocol_n
    {
        enum class Protocol {
            HTTP1_1,
            NONE // For error handling
        };
    }

    enum class Code : uint16_t {
        OK           = 200,
        CREATED      = 201,
        NO_CONTENT   = 204,

        BAD_REQUEST  = 400,
        UNAUTHORIZED = 401,
        FORBIDDEN    = 403,
        NOT_FOUND    = 404,

        SERVER_ERROR = 500,

        NONE = 0 // For error handling
    };

    const std::unordered_map<Code, std::string> STATUS_REASONS = {
        { Code::OK,             "OK" },
        { Code::CREATED,        "Created" },
        { Code::NO_CONTENT,     "No Content" },

        { Code::BAD_REQUEST,    "Bad Request" },
        { Code::UNAUTHORIZED,   "Unauthorized" },
        { Code::FORBIDDEN,      "Forbidden" },
        { Code::NOT_FOUND,      "Not Found" },

        { Code::SERVER_ERROR,   "Internal Server Error" },
    };

    // TODO: Overload operator==, for int and for Code.
    struct Status_s : public StringConvertible {
    public:
        Status_s(Code code) : m_code(code), m_reason(STATUS_REASONS.at(code)) {}

        Code code() const { return m_code; }
        std::string reason() const { return m_reason; }
        std::string toString() const override { return std::format("{} {}", static_cast<uint16_t>(m_code), m_reason); }

    private:
        Code m_code;
        std::string m_reason;
    };
}