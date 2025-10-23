#include <Time.h>

std::chrono::time_point<std::chrono::system_clock> Time::Now() {
    return std::chrono::system_clock::now();
}

std::string Time::ToUTC(std::chrono::time_point<std::chrono::system_clock> Time) {
    return std::format("{:%a, %d %b %Y %H:%M:%S GMT}", std::chrono::floor<std::chrono::seconds>(Time));;
}