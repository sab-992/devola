#pragma once

#include <Body.h>
#include <format>
#include <Net.h>
#include <nlohmann/json.hpp>
#include <regex>
#include <string>
#include <Trim.h>

namespace WS_n
{
    template<typename T>
    class Body : public Net_n::Body_c<T> {
    public:
        Body() {}

        static WS_n::Body<T> Parse(std::string RawBody) {
            if constexpr (std::is_same_v<T, nlohmann::json>)
                return WS_n::Body<T>(json::parse(RawBody));
            return WS_n::Body<T>(RawBody);
        }

        static WS_n::Body<T> Build(T MessageBody) {
            return WS_n::Body<T>(MessageBody);
        }

    private:
        Body(T MessageBody) {
            Net_n::Body_c<T>::m_Body = MessageBody;
        }
    };
}