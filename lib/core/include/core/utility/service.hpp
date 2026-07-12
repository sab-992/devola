#pragma once

#include <memory>


template <typename Derived>
concept HasCreateMethod = requires { Derived::create; };

// Trick to have only one allocation while creating a unique_ptr and still have a private constructor for Derived class.
template <typename Derived>
class Service {
protected:
    struct Private_s {};

    Service() {
        static_assert(HasCreateMethod<Derived>, "Derived class must implement \"static std::unique_ptr<Derived> create()\"");
    }

public:
    Service(const Service&) = delete;
    Service& operator=(const Service&) = delete;

    friend std::unique_ptr<Derived> std::make_unique<Derived>();
};