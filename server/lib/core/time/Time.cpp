#include <Time.h>

Time::Time() {
    m_CreationTimePoint = Now();
}

std::chrono::time_point<std::chrono::system_clock> Time::Now() {
    return std::chrono::system_clock::now();
}

std::string Time::ToUTC() {
    return std::format("{:%a, %d %b %Y %H:%M:%S GMT}", std::chrono::floor<std::chrono::seconds>(Now()));;
}