#pragma once

#include <memory>


template<typename T>
bool pointersEqual(const std::unique_ptr<T>& lhs,const std::unique_ptr<T>& rhs) {
    if(lhs == rhs)
        return true;
    if(lhs && rhs)
        return *lhs == *rhs;
    return false;
}

template<typename T>
bool pointersEqual(const std::shared_ptr<T>& lhs,const std::shared_ptr<T>& rhs) {
    if(lhs == rhs)
        return true;
    if(lhs && rhs)
        return *lhs == *rhs;
    return false;
}