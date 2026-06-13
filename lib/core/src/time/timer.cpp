#include <core/time/timer.h>


time_n::Timer::Timer(const Private&, asio::io_context* ioCtx, timerId_t identifier, const TimerOptions& options) : m_id(identifier), m_options(options) {
    m_timer = std::make_unique<asio::steady_timer>(*ioCtx);
}

std::shared_ptr<time_n::Timer> time_n::Timer::create(asio::io_context* ioCtx, timerId_t identifier, const TimerOptions& options) {
    return std::make_shared<Timer>(Private(), ioCtx, identifier, options);
}

time_n::timerId_t time_n::Timer::id() const {
    return m_id;
}

time_n::asioCallback_t time_n::Timer::onTimeExpiryCallback() {
    return [self = shared_from_this()](const asio::error_code& ec) {
        if (ec == asio::error::operation_aborted)
            return;

        self->m_options.callback(self->m_id, ec);

        if (self->m_options.keepRestarting)
            self->start();
    };
}

void time_n::Timer::start() {
    m_timer->expires_after(m_options.duration);
    m_timer->async_wait(onTimeExpiryCallback());
}

void time_n::Timer::stop() {
    m_options.keepRestarting = false;
    m_timer->cancel();
}