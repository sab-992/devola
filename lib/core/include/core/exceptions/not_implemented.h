#pragma once

#include <stdexcept>
#include <string>


namespace except_n
{
    class NotImplemented : public std::logic_error {
    public:
        NotImplemented(std::string reason = "Function not implemented")
        : std::logic_error(reason) {}
    };
}