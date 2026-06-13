#pragma once

#include <asio.hpp>
#include <core/exception.h>
#include <core/time/timer.h>
#include <chrono>
#include <string>
#include <unordered_map>


class Time {
public:
    Time(asio::io_context* ioCtx);
    ~Time();

    template<typename Duration=std::chrono::milliseconds>
    static std::string format(std::string_view specification, const std::chrono::time_point<std::chrono::system_clock>& time) {
        auto flooredDuration = std::chrono::floor<Duration>(time);
        return std::vformat(specification, std::make_format_args(flooredDuration));
    }

    static std::chrono::time_point<std::chrono::system_clock> now();
    static std::string toUTC(const std::chrono::time_point<std::chrono::system_clock>& time);

    time_n::timerId_t startTimer(const time_n::TimerOptions& options);
    void stopTimer(time_n::timerId_t identifier);

private:
    asio::io_context* m_ioCtx;
    std::unordered_map<time_n::timerId_t, std::shared_ptr<time_n::Timer>> m_timers;

    static time_n::timerId_t nextID();

    std::shared_ptr<time_n::Timer> timer(time_n::timerId_t identifier);

    void validateTimer(time_n::timerId_t identifier);
};