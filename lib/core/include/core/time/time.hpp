#pragma once

#include <asio.hpp>
#include <core/exception.hpp>
#include <core/time/timer.hpp>
#include <chrono>
#include <string>
#include <unordered_map>
#include <sstream>


class Time {
public:
    Time(asio::io_context* ioCtx);
    ~Time();

    time_n::timerId_t startTimer(const time_n::TimerOptions& options);
    void stopTimer(time_n::timerId_t identifier);

    static std::chrono::time_point<std::chrono::system_clock> fromSecondSinceEpoch(double timepoint);
    static std::chrono::time_point<std::chrono::system_clock> now();
    static std::chrono::time_point<std::chrono::system_clock> timepoint(const std::string& specification, std::string_view time);
    static double toSecondsSinceEpoch(const std::chrono::time_point<std::chrono::system_clock>& timepoint);
    static std::string toUTC(const std::chrono::time_point<std::chrono::system_clock>& time);

    template<typename Duration=std::chrono::seconds>
    static std::string format(std::string_view specification, const std::chrono::time_point<std::chrono::system_clock>& time) {
        auto flooredDuration = std::chrono::floor<Duration>(time);
        return std::vformat(specification, std::make_format_args(flooredDuration));
    }

private:
    asio::io_context* m_ioCtx;
    std::unordered_map<time_n::timerId_t, std::shared_ptr<time_n::Timer>> m_timers;

    std::shared_ptr<time_n::Timer> timer(time_n::timerId_t identifier);
    void validateTimer(time_n::timerId_t identifier);

    static time_n::timerId_t nextID();
};