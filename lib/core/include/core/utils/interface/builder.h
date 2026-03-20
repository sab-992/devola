#pragma once


template <typename Derived>
class Builder_i {
public:
    virtual Derived& build() & = 0;
    virtual Derived build() && = 0;
};