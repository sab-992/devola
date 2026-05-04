#pragma once

#include <memory>


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