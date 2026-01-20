#pragma once

#include <core/str/string_formattable.h>
#include <format>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>


using HeadersUMap_t = std::unordered_map<std::string, std::string>;
using json = nlohmann::json;

namespace network_n
{   
    enum class Code : uint16_t {
        /* --------- 200 --------- */
        OK = 200,
        CREATED = 201,
        ACCEPTED = 202,
        NO_CONTENT = 204,
        /* --------- 400 --------- */
        BAD_REQUEST = 400,
        UNAUTHORIZED = 401,
        FORBIDDEN = 403,
        NOT_FOUND = 404,
        METHOD_NOT_ALLOWED = 405,
        CONFLICT = 409,
        GONE = 410,
        UNPROCESSABLE_ENTITY = 422,
        TOO_MANY_REQUESTS = 429,
        /* --------- 500 --------- */
        INTERNAL_SERVER_ERROR = 500,
        NOT_IMPLEMENTED = 501,
        BAD_GATEWAY = 502,
        SERVICE_UNAVAILABLE = 503,
        GATEWAY_TIMEOUT = 504
    };

    const std::unordered_map<network_n::Code, std::string> STATUS_REASONS = {
        /* --------- 200 --------- */
        { network_n::Code::OK,                      "OK" },
        { network_n::Code::CREATED,                 "Created" },
        { network_n::Code::ACCEPTED,                "Accepted" },
        { network_n::Code::NO_CONTENT,              "No Content" },
        /* --------- 400 --------- */
        { network_n::Code::BAD_REQUEST,             "Bad Request" },
        { network_n::Code::UNAUTHORIZED,            "Unauthorized" },
        { network_n::Code::FORBIDDEN,               "Forbidden" },
        { network_n::Code::NOT_FOUND,               "Not Found" },
        { network_n::Code::METHOD_NOT_ALLOWED,      "Method Not Allowed" },
        { network_n::Code::CONFLICT,                "Conflict" },
        { network_n::Code::GONE,                    "Gone" },
        { network_n::Code::UNPROCESSABLE_ENTITY,    "Unprocessable Entity" },
        { network_n::Code::TOO_MANY_REQUESTS,       "Too Many Requests" },
        /* --------- 500 --------- */
        { network_n::Code::INTERNAL_SERVER_ERROR,   "Internal Server Error" },
        { network_n::Code::NOT_IMPLEMENTED,         "Not Implemented" },
        { network_n::Code::BAD_GATEWAY,             "Bad Gateway" },
        { network_n::Code::SERVICE_UNAVAILABLE,     "Service Unavailable" },
        { network_n::Code::GATEWAY_TIMEOUT,         "Gateway Timeout"}
    };

    // TODO: Overload operator==.
    struct Endpoint : virtual public StringFormattable_i {
    public:
        Endpoint() {}
        Endpoint(std::string host, uint16_t port)
        : m_host(host), m_port(port) {}

        std::string host() const { return m_host; }
        uint16_t port() const { return m_port; }

        void setHost(std::string host) { m_host = host; }
        void setPort(uint16_t port) { m_port = port; }

    protected:
        std::string toString() const override { return std::format("{}:{}", m_host, m_port); }

    private:
        std::string m_host;
        uint16_t m_port;
    };

    // TODO: Overload operator==, for int and for network_n::Code.
    class Status : virtual public StringFormattable_i {
    public:
        Status() {}

        Status(network_n::Code code)
        : m_code(static_cast<uint16_t>(code)), m_reason(STATUS_REASONS.at(code)) {}

        uint16_t code() { return m_code; }

        std::string reason() { return m_reason; }

        void updateStatus(network_n::Code code) { m_code = static_cast<uint16_t>(code); m_reason = STATUS_REASONS.at(code); }
    
    protected:
        std::string toString() const override { return std::format("{} {}", m_code, m_reason); }

    private:
        uint16_t m_code;
        std::string m_reason;
    };
}