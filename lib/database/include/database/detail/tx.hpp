#pragma once

namespace database_n
{
    class Tx {
    public:
        virtual ~Tx() = default;

        Tx(const Tx&) = delete;
        Tx& operator=(const Tx&) = delete;

    protected:
        Tx() = default;

        Tx(Tx&&) = default;
        Tx& operator=(Tx&&) = default;
    };
}
