#pragma once

#include <functional>


namespace thread_n
{
    using completionToken_t = std::function<void()>;
}