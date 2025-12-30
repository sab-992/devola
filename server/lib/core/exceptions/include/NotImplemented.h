#pragma once

#include <stdexcept>
#include <string>

namespace Except_n
{
    class NotImplemented : public std::logic_error {
    public:
        NotImplemented(std::string Reason = "Function not implemented")
        : std::logic_error(Reason) {}
    };
}