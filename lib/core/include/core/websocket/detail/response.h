#pragma once

#include <core/network/detail/message.h>
#include <core/network/network.h>
#include <core/websocket/detail/body.h>
#include <core/websocket/detail/headers.h>
#include <core/websocket/interface/response.h>
#include <memory>
#include <string>


namespace websocket_n
{
    template<typename T>
    class Response : public network_n::Message_c<T, websocket_n::Headers, websocket_n::Body<T>>, public websocket_n::Response_i<T> {
    public:
        ~Response() {}

        static std::unique_ptr<websocket_n::Response_i<T>> create(std::string rawResponse) {
            return std::unique_ptr<websocket_n::Response<T>>(new websocket_n::Response<T>(rawResponse));
        }

        template<typename U = T>
        static std::unique_ptr<websocket_n::Response_i<T>> create(network_n::Code statusCode, const HeadersUMap_t& headersMap, U&& body = T{}) {
            return std::unique_ptr<websocket_n::Response<T>>(new websocket_n::Response<T>(statusCode, headersMap, body));
        }

        std::string apiEndpoint() const override { return this->m_headers.apiEndpoint(); }

        network_n::Status status() const override { return this->m_headers.status(); }

    protected:
        std::string toString() const override { return network_n::Message_c<T, websocket_n::Headers, websocket_n::Body<T>>::toString(); }

    private:
        Response(std::string response) {
            std::pair<std::string, std::string> splitMessage = this->split(response);
            this->initialize(websocket_n::Headers(splitMessage.first), websocket_n::Body<T>::parse(splitMessage.second));
        }

        Response(network_n::Code statusCode, const HeadersUMap_t& headersMap, T body) {
            this->initialize(websocket_n::Headers(statusCode, headersMap), websocket_n::Body<T>::build(body));
        }
    };
}
