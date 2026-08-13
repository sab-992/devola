#include <core/time/time.hpp>


Time::Time(asio::io_context* ioCtx) : m_ioCtx(ioCtx) {}

Time::~Time() {
    if (m_timers.empty()) return;

    for (auto& [id, _] : m_timers)
        stopTimer(id);
}

std::chrono::time_point<std::chrono::system_clock> Time::fromSecondSinceEpoch(double seconds) {
    return  std::chrono::time_point<std::chrono::system_clock>(
                std::chrono::duration_cast<std::chrono::system_clock::duration>(
                    std::chrono::duration<double>(seconds)));
}

time_n::timerId_t Time::nextID() {
    static time_n::timerId_t currentID = 0;
    return currentID++;
}

std::chrono::time_point<std::chrono::system_clock> Time::now() {
    return std::chrono::system_clock::now();
}

time_n::timerId_t Time::startTimer(const time_n::TimerOptions& options) {
    if (not m_ioCtx)
        throw Exception("Timer's I/O context is nullptr");

    time_n::timerId_t id = nextID();

    const auto& [it, _] = m_timers.emplace(id, time_n::Timer::create(m_ioCtx, id, options));

    it->second->start();

    return id;
}

void Time::stopTimer(time_n::timerId_t identifier) {
    timer(identifier)->stop();
    m_timers.erase(identifier);
    rehashIfNeeded(m_timers);
}

std::chrono::time_point<std::chrono::system_clock> Time::timepoint(const std::string& specification, std::string_view time) {
    std::istringstream in{std::string(time)};
    std::chrono::sys_seconds tp;
    in >> std::chrono::parse(specification, tp);
    return tp;
}

std::shared_ptr<time_n::Timer> Time::timer(time_n::timerId_t identifier) {
    validateTimer(identifier);
    return m_timers.at(identifier);
}

double Time::toSecondsSinceEpoch(const std::chrono::time_point<std::chrono::system_clock>& timepoint) {
    return std::chrono::duration<double>(timepoint.time_since_epoch()).count();
}

std::string Time::toUTC(const std::chrono::time_point<std::chrono::system_clock>& time) {
    return format<std::chrono::seconds>("{:%a, %d %b %Y %H:%M:%S GMT}", time);
}

void Time::validateTimer(time_n::timerId_t identifier) {
    if (not m_timers.contains(identifier))
        throw InvalidArgument("does not exist", "Timer");
}