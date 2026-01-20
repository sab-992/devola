#include <core/time/time.h>

std::chrono::time_point<std::chrono::system_clock> Time::now() {
    return std::chrono::system_clock::now();
}

std::string Time::toUTC(std::chrono::time_point<std::chrono::system_clock> time) {
    return std::format("{:%a, %d %b %Y %H:%M:%S GMT}", std::chrono::floor<std::chrono::seconds>(time));;
}