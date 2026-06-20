#include <core/thread/thread.hpp>


Thread::Thread(std::thread&& thread) : m_id(thread.get_id()), m_status(thread_n::Status_en::RUNNING) {
    m_thread = std::move(thread);
}

Thread::~Thread() {
    if (not m_thread.joinable()) return;

    m_thread.join();
    m_status = thread_n::Status_en::JOINED;
}

std::thread::id Thread::id() const {
    return m_id;
}

thread_n::Status_en Thread::status() const {
    return m_status;
}