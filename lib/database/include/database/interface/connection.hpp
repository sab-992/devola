#pragma once


namespace database_n
{
    class Connectable_i {
    public:
        virtual ~Connectable_i() = default;
        virtual bool connect() = 0;
        virtual void disconnect() = 0;
        virtual bool isConnected() const = 0;
    };
}
