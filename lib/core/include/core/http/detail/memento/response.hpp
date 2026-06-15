#pragma once

#include <core/http/response.hpp>
#include <core/network/detail/body.hpp>
#include <core/network/detail/headers.hpp>
#include <core/network/interface/protocol.hpp>
#include <core/network/network.hpp>
#include <core/utility/interface/memento.hpp>
#include <core/utility/compare.hpp>
#include <memory>


namespace http_n
{
    template <typename T>
    class Response;

    namespace memento_n
    {
        template <typename T>
        class Response : public Memento_i<Response<T>> {
        public:
            Response(http_n::Response<T> toSave) {
                using namespace network_n;

                m_body = std::make_unique<Body<T>>(*toSave.m_body);
                m_headers = std::make_unique<Headers>(*toSave.m_headers);
                m_protocol = toSave.m_protocol;
                m_status = toSave.m_status;
            }

            Response(const Response<T>& other) {
                using namespace network_n;

                m_body = std::make_unique<Body<T>>(*other.m_body);
                m_headers = std::make_unique<Headers>(*other.m_headers);
                m_protocol = other.m_protocol;
                m_status = other.m_status;
            }

            ~Response() = default;

            friend std::unique_ptr<Response> std::make_unique<Response>();

            bool operator==(const Response<T>& other) const override {
                return pointersEqual(m_body, other.m_body)       and
                       pointersEqual(m_headers, other.m_headers) and
                       m_protocol == other.m_protocol            and
                       m_status   == other.m_status;
            }

        private:
            std::unique_ptr<network_n::Body<T>> m_body;
            std::unique_ptr<network_n::Headers> m_headers;
            std::shared_ptr<network_n::protocol_n::Protocol_i<T>> m_protocol;
            network_n::Status_s m_status;
        };
    }
}