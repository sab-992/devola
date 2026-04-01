#pragma once


template <typename Derived>
class Builder_i {
public:
    virtual ~Builder_i() = default;

    virtual Derived& build() & = 0;
    virtual Derived build() && = 0;
};