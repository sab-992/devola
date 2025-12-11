#pragma once

#include <NetCommon.h>
#include <string>

class Headers_i {
public:
    virtual ~Headers_i() = default;

    virtual Net_n::Endpoint Endpoint() = 0;
    virtual std::string GetHeader(std::string Header) = 0;
    virtual HeadersUMap_t Map() = 0;
    virtual std::string Raw() = 0;
    virtual Net_n::Status Status() = 0;
    virtual std::string ToString() = 0;
};

class Headers_c : public Headers_i {
public:
    ~Headers_c() = default;

    Net_n::Endpoint Endpoint() override;
    std::string GetHeader(std::string Header) override;
    HeadersUMap_t Map() override;
    std::string Raw() override;
    Net_n::Status Status() override;
    std::string ToString() override;
protected:
    Net_n::Endpoint m_Endpoint;
    std::string m_Headers;
    HeadersUMap_t m_HeadersMap;
    Net_n::Status m_Status;
};