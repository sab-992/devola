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
    class Response;

    namespace memento_n
    {
        class Response : public Memento_i<Response> {
        public:
            Response(http_n::Response toSave);
            Response(const Response& other);
            ~Response() = default;

            friend std::unique_ptr<Response> std::make_unique<Response>();

            bool operator==(const Response& other) const override;

        private:
            std::unique_ptr<network_n::Body> m_body;
            std::unique_ptr<network_n::Headers> m_headers;
            std::shared_ptr<network_n::protocol_n::Protocol_i> m_protocol;
            network_n::Status_s m_status;
        };
    }
}