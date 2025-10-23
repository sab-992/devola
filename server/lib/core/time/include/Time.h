#pragma once

#include <chrono>
#include <format>
#include <iomanip>
#include <iostream>
#include <string>

class Time {
public:
    Time() = delete;

    static std::chrono::time_point<std::chrono::system_clock> Now();
    static std::string ToUTC(std::chrono::time_point<std::chrono::system_clock> Time);
private:
    std::chrono::time_point<std::chrono::system_clock> m_CreationTimePoint;
};