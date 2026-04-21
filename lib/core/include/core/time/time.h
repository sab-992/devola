#pragma once

#include <chrono>
#include <format>
#include <iomanip>
#include <string>


class Time {
public:
    Time() = delete;

    static std::chrono::time_point<std::chrono::system_clock> now();
    static std::string toUTC(std::chrono::time_point<std::chrono::system_clock> time);
};