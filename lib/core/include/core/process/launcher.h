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
        Process& launch(const std::string& command) {
            processId_t pid = fork();

            if (pid < 0) throw Exception("Error while creating process");
            if (pid == 0) { std::system(command.c_str()); std::exit(0); }

            auto [it, inserted] = m_processes.emplace(pid, pid);
            return it->second;
        }

        template <typename F, typename... Args> requires std::invocable<F, Args...>
        Process& launch(F&& function, Args&&... args) {
            processId_t pid = fork();

            if (pid < 0) throw Exception("Error while creating process");
            if (pid == 0) { std::invoke(std::forward<F>(function), std::forward<Args>(args)...); std::exit(0); }

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