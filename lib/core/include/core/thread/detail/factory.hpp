#pragma once

#include <core/thread/detail/settings.hpp>
#include <core/thread/thread.hpp>
#include <core/thread/registry.hpp>

class Thread;

namespace thread_n
{
    class Registry;

    class Factory {
    public:
        Factory() = delete;
        ~Factory() = default;

        friend class Registry;

    private:
        static Thread create(thread_n::callback_t function);
    };
}