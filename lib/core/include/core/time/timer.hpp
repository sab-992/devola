#pragma once

#include <asio.hpp>
#include <core/exception.hpp>
#include <core/utility/rehash.hpp>
#include <chrono>


namespace time_n
{
    using timerId_t = size_t;
    using asioCallback_t = std::function<void(const asio::error_code&)>;
    using callback_t = std::function<void(timerId_t, const asio::error_code&)>;

    struct TimerOptions {
        template<typename Rep, typename Period>
        TimerOptions(std::chrono::duration<Rep, Period> duration_, callback_t callback_, bool keepRestarting_=false) {
            duration = std::chrono::duration_cast<std::chrono::nanoseconds>(duration_);
            callback = callback_;
            keepRestarting = keepRestarting_;
        }

        std::chrono::nanoseconds duration;
        callback_t callback;
        bool keepRestarting;
    };

    class Timer : public std::enable_shared_from_this<Timer> {
    private:
        struct Private_s { explicit Private_s() = default; };

    public:
        Timer(const Private_s&, asio::io_context* ioCtx, timerId_t identifier, const TimerOptions& options);

        static std::shared_ptr<Timer> create(asio::io_context* ioCtx, timerId_t identifier, const TimerOptions& options);

        timerId_t id() const;

        void start();
        void stop();

    private:
        timerId_t m_id;
        TimerOptions m_options;
        std::unique_ptr<asio::steady_timer> m_timer;

        asioCallback_t onTimeExpiryCallback();
    };
}
