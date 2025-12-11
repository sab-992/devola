#include <Headers.h>


Net_n::Endpoint Headers_c::Endpoint() {
    return m_Endpoint;
};

std::string Headers_c::GetHeader(std::string Header) {
    return m_HeadersMap.contains(Header) ? m_HeadersMap[Header] : "";
};

HeadersUMap_t Headers_c::Map() {
    return m_HeadersMap;
};

Net_n::Status Headers_c::Status() {
    return m_Status;
};

std::string Headers_c::ToString() {
    return m_Headers;
};

std::string Headers_c::Raw() {
    return m_Headers;
};