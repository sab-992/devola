#pragma once

#include <chrono>
#include <format>
#include <iomanip>
#include <iostream>
#include <string>

class Time_i {
public:
    virtual ~Time_i() = default;

    virtual std::chrono::time_point<std::chrono::system_clock> Now() = 0;
    virtual std::string ToUTC() = 0;
};

class Time : Time_i {
public:
    Time();

    std::chrono::time_point<std::chrono::system_clock> Now() override;
    std::string ToUTC() override;
private:
    std::chrono::time_point<std::chrono::system_clock> m_CreationTimePoint;
};