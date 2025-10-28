#pragma once

#include <HttpCommon.h>
#include <nlohmann/json.hpp>
#include <string>


const unsigned int SPACES_FOR_INDENT = 4;

template<typename T>
class HttpBody {
public:
    HttpBody(json Body) { Build(Body); };
    template<typename U = T>
    HttpBody(U&& RawBody = T{}) { Parse(std::forward<U>(RawBody)); };

    std::string AsString() { 
        if constexpr (std::is_same_v<T, nlohmann::json>)
            return m_Body.dump(SPACES_FOR_INDENT);
        return m_Body;
    };
    
    T Body() { return m_Body; };
    std::string Raw() { return m_RawBody; };
private:
    T m_Body;
    std::string m_RawBody;

    void Parse(std::string RawBody) {
        m_RawBody = RawBody;
        if constexpr (std::is_same_v<T, nlohmann::json>)
            m_Body = json::parse(RawBody);
        else m_Body = RawBody;
    };

    void Build(json Body) {
        m_Body = Body;
    };
};