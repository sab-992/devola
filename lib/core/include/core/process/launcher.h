#pragma once

#include <core/exception.h>
#include <core/process/process.h>
#include <cstdlib>
#include <unordered_map>


class Process;

namespace process_n
{
    class Launcher {
    public:
        Launcher() {}

        ~Launcher() {
            for (auto& [key, process] : m_processes)
                process.terminate();

            for (auto& [key, process] : m_processes)
                process.wait();
        }

        #ifdef __linux__
        Process& launch(std::string_view command) {
            processId_t pid = fork();

            const std::string background_process_cmd = std::format("{} &", command);

            if (pid < 0) throw Exception("Error while creating process");
            if (pid == 0) { std::system(background_process_cmd.c_str()); std::exit(0); }

            auto [it, inserted] = m_processes.emplace(pid, pid);
            return it->second;
        }
        #endif

        #ifdef _WIN32
            /* TODO */
        #endif
    private:
        inline static std::unordered_map<processId_t, Process> m_processes;
    };
}