#pragma once

#include <core/http/detail/body.h>
#include <core/http/detail/headers.h>
#include <core/http/interface/response.h>
#include <core/network/detail/message.h>
#include <core/network/network.h>
#include <memory>
#include <string>


namespace http_n
{
    template<typename T>
    class Response : public network_n::Message_c<T, http_n::Headers, http_n::Body<T>>, public http_n::Response_i<T> {
    public:
        ~Response() {};

        static std::unique_ptr<http_n::Response_i<T>> create(std::string rawResponse) {
            return std::unique_ptr<http_n::Response<T>>(new http_n::Response<T>(rawResponse));
        }

        template<typename U = T>
        static std::unique_ptr<http_n::Response_i<T>> create(network_n::Code statusCode, const HeadersUMap_t& headersMap, U&& body = T{}, bool chunkMessage = false) {
            return std::unique_ptr<http_n::Response<T>>(new http_n::Response<T>(statusCode, headersMap, body, chunkMessage));
        }

        network_n::Status status() const override { return this->m_headers.status(); }

    protected:
        std::string toString() const override { return network_n::Message_c<T, http_n::Headers, http_n::Body<T>>::toString(); }

    private:
        Response(std::string response) {
            std::pair<std::string, std::string> splitMessage = this->split(response);
            http_n::Headers headers(splitMessage.first);
            this->initialize(headers, http_n::Body<T>::parse(splitMessage.second, headers.getHeader(http_n::TRANSFER_ENCODING)));
        }

        Response(network_n::Code statusCode, const HeadersUMap_t& headersMap, T body, bool chunkMessage) {
            this->initialize(http_n::Headers(statusCode, headersMap), http_n::Body<T>::build(body, chunkMessage));
        }
    };
}