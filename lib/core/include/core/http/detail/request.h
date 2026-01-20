#pragma once

#include <core/http/detail/body.h>
#include <core/http/detail/headers.h>
#include <core/http/interface/request.h>
#include <core/network/detail/message.h>
#include <core/network/interface/request.h>
#include <core/network/network.h>
#include <memory>
#include <string>


namespace http_n
{
    template<typename T>
    class Request : public network_n::Message_c<T, http_n::Headers, http_n::Body<T>>, public http_n::Request_i<T> {
    public:
        ~Request() {}

        static std::unique_ptr<http_n::Request_i<T>> create(std::string rawRequest) {
            return std::unique_ptr<http_n::Request<T>>(new http_n::Request<T>(rawRequest));
        }

        template<typename U = T>
        static std::unique_ptr<http_n::Request_i<T>> create(std::string method, std::string apiEndpoint, const network_n::Endpoint& networkEndpoint, const HeadersUMap_t& headersMap, U&& body = T{}, bool chunkMessage = false) {
            return std::unique_ptr<http_n::Request<T>>(new http_n::Request<T>(method, apiEndpoint, networkEndpoint, headersMap, std::forward<U>(body), chunkMessage));
        }

        std::string apiEndpoint() const override { return this->m_headers.apiEndpoint(); }

        std::string method() const override { return this->m_headers.method(); }

    protected:
        std::string toString() const override { return network_n::Message_c<T, http_n::Headers, http_n::Body<T>>::toString(); }

    private:
        Request(std::string request) {
            std::pair<std::string, std::string> splitMessage = this->split(request);
            http_n::Headers headers(splitMessage.first);
            this->initialize(headers, http_n::Body<T>::parse(splitMessage.second, headers.getHeader(http_n::TRANSFER_ENCODING)));
        }

        Request(std::string method, std::string apiEndpoint, const network_n::Endpoint& networkEndpoint, const HeadersUMap_t& headersMap, T body, bool chunkMessage) {
            this->initialize(http_n::Headers(method, apiEndpoint, networkEndpoint, headersMap), http_n::Body<T>::build(body, chunkMessage));
        }
    };
}