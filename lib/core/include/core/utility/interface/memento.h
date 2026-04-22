#pragma once


#include <memory>

template <typename Derived>
class Memento_i {
public:
    virtual ~Memento_i() = default;

    virtual bool operator==(const Derived& other) const = 0;
};