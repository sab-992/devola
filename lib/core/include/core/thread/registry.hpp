#pragma once

#include <core/exception.hpp>
#include <core/thread/detail/factory.hpp>
#include <core/thread/detail/settings.hpp>
#include <core/thread/thread.hpp>
#include <core/utility/singleton.hpp>
#include <core/utility/rehash.hpp>
#include <thread>


class Thread;

namespace thread_n
{
    class Factory;

    class Registry : public Singleton<Registry> {
    public:
        Registry(const Private_s&);
        ~Registry();

        const Thread& start(thread_n::callback_t function);
        void join(std::thread::id id);

    private:
        inline static std::unordered_map<std::thread::id, Thread> m_threads;
    };
}