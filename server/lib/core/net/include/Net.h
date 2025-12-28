#pragma once

#include <concepts>
#include <format>
#include <nlohmann/json.hpp>
#include <Str.h>
#include <string>
#include <unordered_map>


using HeadersUMap_t = std::unordered_map<std::string, std::string>;
using json = nlohmann::json;


namespace Net_n
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

    const std::unordered_map<Net_n::Code, std::string> STATUS_REASONS = {
        /* --------- 200 --------- */
        { Net_n::Code::OK,                      "OK" },
        { Net_n::Code::CREATED,                 "Created" },
        { Net_n::Code::ACCEPTED,                "Accepted" },
        { Net_n::Code::NO_CONTENT,              "No Content" },
        /* --------- 400 --------- */
        { Net_n::Code::BAD_REQUEST,             "Bad Request" },
        { Net_n::Code::UNAUTHORIZED,            "Unauthorized" },
        { Net_n::Code::FORBIDDEN,               "Forbidden" },
        { Net_n::Code::NOT_FOUND,               "Not Found" },
        { Net_n::Code::METHOD_NOT_ALLOWED,      "Method Not Allowed" },
        { Net_n::Code::CONFLICT,                "Conflict" },
        { Net_n::Code::GONE,                    "Gone" },
        { Net_n::Code::UNPROCESSABLE_ENTITY,    "Unprocessable Entity" },
        { Net_n::Code::TOO_MANY_REQUESTS,       "Too Many Requests" },
        /* --------- 500 --------- */
        { Net_n::Code::INTERNAL_SERVER_ERROR,   "Internal Server Error" },
        { Net_n::Code::NOT_IMPLEMENTED,         "Not Implemented" },
        { Net_n::Code::BAD_GATEWAY,             "Bad Gateway" },
        { Net_n::Code::SERVICE_UNAVAILABLE,     "Service Unavailable" },
        { Net_n::Code::GATEWAY_TIMEOUT,         "Gateway Timeout"}
    };

    struct NetworkEndpoint : public StringFormattable_i {
    public:
        NetworkEndpoint() {}
        NetworkEndpoint(std::string Host, uint16_t Port)
        : m_Host(Host), m_Port(Port) {}

        std::string Host() const { return m_Host; }
        uint16_t Port() const { return m_Port; }

        void SetHost(std::string Host) { m_Host = Host; }
        void SetPort(uint16_t Port) { m_Port = Port; }

        std::string ToString() const override { return std::format("{}:{}", m_Host, m_Port); }
    private:
        std::string m_Host;
        uint16_t m_Port;
    };

    class Status : public StringFormattable_i {
    public:
        Status() {}

        Status(Net_n::Code Code)
        : m_Code(static_cast<uint16_t>(Code)), m_Reason(STATUS_REASONS.at(Code)) {}

        uint16_t Code() { return m_Code; }

        std::string Reason() { return m_Reason; }

        std::string ToString() const override { return std::format("{} {}", m_Code, m_Reason); }

        void UpdateStatus(Net_n::Code Code) { m_Code = static_cast<uint16_t>(Code); m_Reason = STATUS_REASONS.at(Code); }

    private:
        uint16_t m_Code;
        std::string m_Reason;
    };

    template<typename T>
    class Body_i : public StringFormattable_i {
    public:
        virtual ~Body_i() = default;

        virtual T Get() const = 0;
    };

    class Headers_i : public StringFormattable_i {
    public:
        virtual ~Headers_i() = default;

        virtual std::string APIEndpoint() const = 0;
        virtual std::string Get() const = 0;
        virtual std::string GetHeader(std::string Header) const = 0;
        virtual HeadersUMap_t Map() const = 0;
        virtual std::string Method() const = 0;
        virtual Net_n::NetworkEndpoint NetworkEndpoint() const = 0;
        virtual Net_n::Status Status() const = 0;
    protected:
        virtual std::string ExtractMessageInformation(std::string RawHeader) = 0;
    };

    template <typename T>
    class Message_i : public StringFormattable_i {
    public:
        virtual ~Message_i() = default;

        virtual std::string APIEndpoint() const = 0;
        virtual T Body() const = 0;
        virtual std::string GetHeader(std::string Header) const = 0;
        virtual std::string Headers() const = 0;
        virtual HeadersUMap_t HeadersMap() const = 0;
        virtual std::string Method() const = 0;
        virtual Net_n::NetworkEndpoint NetworkEndpoint() const = 0;
        virtual Net_n::Status Status() const = 0;
    };

    template <typename U>
    concept IsHeader_cpt = std::derived_from<U, Net_n::Headers_i>;

    template<typename U, typename T>
    concept IsBody_cpt = std::derived_from<U, Net_n::Body_i<T>>;
}