#pragma once

#include <core/network/interface/message.h>
#include <core/network/interface/request.h>


namespace websocket_n
{
    template<typename T>
    class Request_i : public network_n::Request_i<T>, virtual public network_n::Message_i<T> {}; 
}