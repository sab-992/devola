#include <core/thread/detail/factory.hpp>


Thread thread_n::Factory::create(thread_n::completionToken_t function) {
    return Thread(std::thread{function});
}