#pragma once

#include <core/network/interface/message.h>
#include <core/network/interface/response.h>


namespace http_n
{
    template<typename T>
    class Response_i : public network_n::Response_i<T>, public virtual network_n::Message_i<T> {};
}