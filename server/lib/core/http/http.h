#include <iostream>

#include <asio.hpp>


class Http {
public:
    Http();
    ~Http();

    void Delete();
    void Get();
    void Head();
    void Options();
    void Patch();
    void Post();
    void Put();
};