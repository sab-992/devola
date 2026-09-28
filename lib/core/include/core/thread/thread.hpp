#pragma once

#include <core/thread/detail/settings.hpp>
#include <core/thread/detail/status.hpp>
#include <core/thread/detail/factory.hpp>
#include <thread>


namespace thread_n { class Factory; }

class Thread {
public:
    ~Thread();
    Thread(Thread&&) = default;

   friend class thread_n::Factory;

   std::thread::id id() const;
   thread_n::Status_en status() const;

private:
    Thread(std::thread&& thread);

    std::thread::id m_id;
    thread_n::Status_en m_status;
    std::thread m_thread;
};