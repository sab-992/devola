#pragma once


class Database_i {
public:
    virtual ~Database_i() = default;

    virtual bool connect() = 0;
};