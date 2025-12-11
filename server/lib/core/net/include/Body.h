#pragma once

#include <string>


const unsigned int SPACES_FOR_INDENT = 4;

template<typename T>
class Body_i {
public:
    virtual ~Body_i() = default;

    virtual T Body() = 0;
    virtual std::string Raw() = 0;
    virtual std::string ToString() = 0;
};

template<typename T>
class Body_c : public Body_i<T> {
public:
    ~Body_c() = default;

    T Body() override { return m_Body; };

    std::string Raw() override { return m_RawBody; };

    std::string ToString() override {
        if constexpr (std::is_same_v<T, nlohmann::json>)
            return m_Body.dump(SPACES_FOR_INDENT);
        return m_Body;
    };

protected:
    T m_Body;
    std::string m_RawBody;
};