#pragma once

#include <core/http/response.h>
#include <core/network/detail/body.h>
#include <core/network/detail/headers.h>
#include <core/network/network.h>
#include <core/utility/interface/memento.h>
#include <core/utility/compare.h>
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
                m_body = toSave.m_body;
                m_headers = toSave.m_headers;
                m_status = toSave.m_status;
            }

            Response(const Response<T>& other) {
                m_body = other.m_body;
                m_headers = other.m_headers;
                m_status = other.m_status;
            }

            ~Response() = default;

            friend std::unique_ptr<Response> std::make_unique<Response>();

            bool operator==(const Response<T>& other) const override {
                return not pointersEqual(m_body, other.m_body)       or
                       not pointersEqual(m_headers, other.m_headers) or
                       m_status !=  other.m_status;
            }

        private:
            std::shared_ptr<network_n::Body<T>> m_body = nullptr;
            std::shared_ptr<network_n::Headers> m_headers = nullptr;
            network_n::Status_s m_status;
        };
    }
}