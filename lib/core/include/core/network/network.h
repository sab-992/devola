#pragma once

#include <cmath>
#include <core/conversion/enum.h>
#include <core/conversion/string_convertible.h>
#include <format>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>


namespace network_n
{
    const int KB = std::pow(10, 3);
    const uint32_t DOWNLOAD_BUFFER_MAX_SIZE = 64 * KB;
    const uint32_t REQUEST_BUFFER_MAX_SIZE = 32 * KB;

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
        NOT_ALLOWED  = 405,
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
        { Code::NOT_ALLOWED,    "Method Not Allowed" },
        { Code::SERVER_ERROR,   "Internal Server Error" },

        { Code::NONE,           "N/A" },
    };

    struct Status_s : public StringConvertible {
    public:
        Status_s(Code code = Code::NONE) : m_code(code), m_reason(STATUS_REASONS.at(code)) {}

        Status_s(const Status_s& other) {
            m_code = other.m_code;
            m_reason = other.m_reason;
        }

        Status_s(Status_s&& other) {
            m_code = std::move(other.m_code);
            m_reason = std::move(other.m_reason);
        }

        ~Status_s() {}

        Status_s& operator=(Status_s other) {
            swap(*this, other);
            return *this;
        }

        bool operator==(const Status_s& other) const { return m_code == other.m_code; }
        bool operator==(const Code& code) const { return m_code == code; }
        bool operator==(uint16_t code) const { return to_underlying(m_code) == code; }

        bool operator!=(const Status_s& other) const { return !(*this == other); }
        bool operator!=(const Code& code) const { return !(*this == code); }
        bool operator!=(uint16_t code) const { return !(*this == code); }

        friend bool operator==(const Code& lhs, const Status_s& rhs) { return rhs == lhs; }
        friend bool operator==(uint16_t lhs, const Status_s& rhs) { return rhs == lhs; }

        Code code() const { return m_code; }
        std::string reason() const { return m_reason; }

        friend void swap(Status_s& lhs, Status_s& rhs) {
            using std::swap;

            swap(lhs.m_code, rhs.m_code);
            swap(lhs.m_reason, rhs.m_reason);
        }

        std::string toString() const override { return std::format("{} {}", to_underlying(m_code), m_reason); }

    private:
        Code m_code;
        std::string m_reason;
    };
}