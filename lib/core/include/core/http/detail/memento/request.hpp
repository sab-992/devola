#pragma once

#include <core/http/request.hpp>
#include <core/network/detail/body.hpp>
#include <core/network/detail/headers.hpp>
#include <core/network/interface/protocol.hpp>
#include <core/network/network.hpp>
#include <core/utility/interface/memento.hpp>
#include <core/utility/compare.hpp>
#include <memory>


namespace http_n
{
    class Request;

    namespace memento_n
    {
        class Request : public Memento_i<Request> {
        public:
            Request(const http_n::Request& toSave);
            Request(const Request& other);
            ~Request() = default;

            friend std::unique_ptr<Request> std::make_unique<Request>();

            bool operator==(const Request& other) const override;

        private:
            std::string m_APIEndpoint;
            std::unique_ptr<network_n::Body> m_body;
            std::unique_ptr<network_n::Headers> m_headers;
            std::string m_method;
            uint16_t m_port;
            std::shared_ptr<network_n::protocol_n::Protocol_i> m_protocol;
            std::string m_URL;
        };
    }
}