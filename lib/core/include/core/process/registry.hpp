#pragma once

#include <core/exception.hpp>
#include <core/process/process.hpp>
#include <core/process/factory.hpp>
#include <core/utility/singleton.hpp>
#include <core/utility/rehash.hpp>
#include <unordered_map>


namespace process_n
{
    class Registry : public Singleton<Registry> {
    public:
        Registry(const Private_s&);
        ~Registry();

        const Process& start(const std::string &executable, std::vector<std::string> stringArgs, bool waitUntilReady=false);
        void stop(processId_t identifier);

    private:
        inline static std::unordered_map<processId_t, Process> m_processes;
    };
}