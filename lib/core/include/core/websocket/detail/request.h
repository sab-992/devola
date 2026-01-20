#pragma once

#include <core/network/detail/message.h>
#include <core/network/network.h>
#include <core/websocket/detail/body.h>
#include <core/websocket/detail/headers.h>
#include <core/websocket/interface/request.h>
#include <memory>
#include <string>


namespace websocket_n
{
    template<typename T>
    class Request : public network_n::Message_c<T, websocket_n::Headers, websocket_n::Body<T>>, public websocket_n::Request_i<T> {
    public:
        ~Request() {}

        static std::unique_ptr<websocket_n::Request_i<T>> create(std::string rawRequest) {
            return std::unique_ptr<websocket_n::Request<T>>(new websocket_n::Request<T>(rawRequest));
        }

        template<typename U = T>
        static std::unique_ptr<websocket_n::Request_i<T>> create(std::string apiEndpoint, const HeadersUMap_t& headersMap, U&& body = T{}) {
            return std::unique_ptr<websocket_n::Request<T>>(new websocket_n::Request<T>(apiEndpoint, headersMap, std::forward<U>(body)));
        }

        std::string apiEndpoint() const override { return this->m_headers.apiEndpoint(); }

    protected:
        std::string toString() const override { return network_n::Message_c<T, websocket_n::Headers, websocket_n::Body<T>>::toString(); }

    private:
        Request(std::string request) {
            std::pair<std::string, std::string> splitMessage = this->split(request);
            this->initialize(websocket_n::Headers(splitMessage.first), websocket_n::Body<T>::parse(splitMessage.second));
        }

        Request(std::string apiEndpoint, const HeadersUMap_t& headersMap, T body) {
            this->initialize(websocket_n::Headers(apiEndpoint, headersMap), websocket_n::Body<T>::build(body));
        }
    };
}