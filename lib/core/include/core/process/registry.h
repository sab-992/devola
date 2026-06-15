#pragma once

#include <core/exception.h>
#include <core/process/process.h>
#include <core/process/factory.h>
#include <core/utility/singleton.h>
#include <core/utility/rehash.h>
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