#pragma once

#include <core/exception.hpp>
#include <core/process/process.hpp>
#include <core/process/detail/factory/factory.hpp>
#include <core/utility/singleton.hpp>
#include <core/utility/rehash.hpp>
#include <unordered_map>


namespace process_n
{
    class Registry : public Singleton<Registry> {
    public:
        Registry(const Private_s&);
        ~Registry();

        std::string receive(processId_t identifier, std::string_view pipe);
        void send(processId_t identifier, std::string_view pipe, std::string_view content);
        const Process& start(const std::string &executable, std::vector<std::string> stringArgs={}, bool waitUntilReady=false);
        void stop(processId_t identifier);

    private:
        inline static std::unordered_map<processId_t, Process> m_processes;

        void validateProcessExists(processId_t identifier);
    };
}