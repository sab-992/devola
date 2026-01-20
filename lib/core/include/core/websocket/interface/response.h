#pragma once

#include <core/network/interface/message.h>
#include <core/network/interface/response.h>


namespace websocket_n
{
    template<typename T>
    class Response_i : public network_n::Response_i<T>, virtual public network_n::Message_i<T> {
    public:
        virtual std::string apiEndpoint() const = 0;
    };
}