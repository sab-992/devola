#pragma once

#include <memory>


// Trick to have only one allocation while creating shared_ptr and still have a private constructor for Derived class.
template <typename Derived>
class Singleton {
protected:
    struct Private_s {};

    Singleton() {};

public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

    static std::shared_ptr<Derived> instance() {
        static std::shared_ptr<Derived> instance = std::make_shared<Derived>(Private_s());
        return instance;
    }
};