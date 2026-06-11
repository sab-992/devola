#pragma once

#include <memory>


// Trick to have only one allocation while creating shared_ptr and still have a private constructor for Derived class.
template <typename Derived>
class Singleton {
protected:
    struct Creator_s {};

public:
    static std::shared_ptr<Derived> instance() {
        static std::shared_ptr<Derived> instance = std::make_shared<Derived>(Creator_s());
        return instance;
    }
};