#pragma once

#include <cmath>
#include <core/conversion/enum.h>
#include <core/conversion/string_convertible.h>
#include <format>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

using headersUMap_t = std::unordered_map<std::string, std::string>;
using startLineInformation_t = std::array<std::string, 3>;


namespace network_n
{
    const int KiB = std::pow(2, 10);
    const size_t DOWNLOAD_BUFFER_MAX_SIZE = 64 * KiB;
    const size_t REQUEST_BUFFER_MAX_SIZE = 32 * KiB;

    namespace protocol_n
    {
        enum class Protocol {
            HTTP1_1,
            NONE // For error handling
        };
    }

    enum class Code : uint16_t {
        OK                 = 200,
        CREATED            = 201,
        NO_CONTENT         = 204,
        MOVED_PERMANENTLY  = 301,
        BAD_REQUEST        = 400,
        UNAUTHORIZED       = 401,
        FORBIDDEN          = 403,
        NOT_FOUND          = 404,
        NOT_ALLOWED        = 405,
        SERVER_ERROR       = 500,

        NONE = 0 // For error handling
    };


    constexpr std::string getReasonFromStatus(Code code) {
        switch (code) {
            case Code::OK:                return "OK";
            case Code::CREATED:           return "Created";
            case Code::NO_CONTENT:        return "No Content";
            case Code::MOVED_PERMANENTLY: return "Moved ";
            case Code::BAD_REQUEST:       return "Bad Request";
            case Code::UNAUTHORIZED:      return "Unauthorized";
            case Code::FORBIDDEN:         return "Forbidden";
            case Code::NOT_FOUND:         return "Not Found";
            case Code::NOT_ALLOWED:       return "Method Not Allowed";
            case Code::SERVER_ERROR:      return "Internal Server Error";
            case Code::NONE:              return "N/A";
            default:                      return "Unknown";
        }
    }

    struct Status_s : public StringConvertible {
    public:
        Status_s(Code code = Code::NONE) : m_code(code), m_reason(getReasonFromStatus(code)) {}

        Status_s(const Status_s& other) = default;
        Status_s(Status_s&& other) = default;

        ~Status_s() = default;

        Status_s& operator=(const Status_s& other) = default;
        Status_s& operator=(Status_s&& other) = default;

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

        std::string toString() const override { return std::format("{} {}", to_underlying(m_code), m_reason); }

    private:
        Code m_code;
        std::string m_reason;
    };
}