#pragma once

#include <asio.hpp>
#include <core/exception.hpp>
#include <core/utility/rehash.hpp>
#include <chrono>


namespace time_n
{
    using timerId_t = size_t;

    struct TimerOptions {
    private:
        using completionToken_t = std::function<void(timerId_t, const asio::error_code&)>;

    public:
        template<typename Rep, typename Period>
        TimerOptions(std::chrono::duration<Rep, Period> duration_, completionToken_t completionToken_, bool keepRestarting_=false) {
            duration = std::chrono::duration_cast<std::chrono::nanoseconds>(duration_);
            completionToken = completionToken_;
            keepRestarting = keepRestarting_;
        }

        std::chrono::nanoseconds duration;
        completionToken_t completionToken;
        bool keepRestarting;
    };

    class Timer : public std::enable_shared_from_this<Timer> {
        struct Private_s { explicit Private_s() = default; };

        using asioCompletionToken_t = std::function<void(const asio::error_code&)>;

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

        asioCompletionToken_t onTimeExpiryCallback();
    };
}
