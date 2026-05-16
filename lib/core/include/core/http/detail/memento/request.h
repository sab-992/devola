#pragma once

#include <core/http/request.h>
#include <core/network/detail/body.h>
#include <core/network/detail/headers.h>
#include <core/network/interface/protocol.h>
#include <core/network/network.h>
#include <core/utility/interface/memento.h>
#include <core/utility/compare.h>
#include <memory>


namespace http_n
{
    template <typename T>
    class Request;

    namespace memento_n
    {
        template <typename T>
        class Request : public Memento_i<Request<T>> {
        public:
            Request(const http_n::Request<T>& toSave) {
                using namespace network_n;

                m_APIEndpoint = toSave.m_APIEndpoint;
                m_body = std::make_unique<Body<T>>(*toSave.m_body);
                m_headers = std::make_unique<Headers>(*toSave.m_headers);
                m_method = toSave.m_method;
                m_port = toSave.m_port;
                m_protocol = toSave.m_protocol;
                m_URL = toSave.m_URL;
            }

            Request(const Request<T>& other) {
                using namespace network_n;

                m_APIEndpoint = other.m_APIEndpoint;
                m_body = std::make_unique<Body<T>>(*other.m_body);
                m_headers = std::make_unique<Headers>(*other.m_headers);
                m_port = other.m_port;
                m_protocol = other.m_protocol;
                m_URL = other.m_URL;
            }

            ~Request() = default;

            friend std::unique_ptr<Request> std::make_unique<Request>();

            bool operator==(const Request<T>& other) const override {
                return m_APIEndpoint !=  other.m_APIEndpoint   or
                       not pointersEqual(m_body, other.m_body)       or
                       not pointersEqual(m_headers, other.m_headers) or
                       m_headers     !=  other.m_headers       or
                       m_method      !=  other.m_method        or
                       m_port        !=  other.m_port          or
                       m_protocol    !=  other.m_protocol      or
                       m_URL         !=  other.m_URL;
            }

        private:
            std::string m_APIEndpoint;
            std::unique_ptr<network_n::Body<T>> m_body;
            std::unique_ptr<network_n::Headers> m_headers;
            std::string m_method;
            uint16_t m_port;
            std::shared_ptr<network_n::protocol_n::Protocol_i<T>> m_protocol;
            std::string m_URL;
        };
    }
}