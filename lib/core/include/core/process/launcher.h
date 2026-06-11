#pragma once

#include <core/exception.h>
#include <core/process/process.h>
#include <core/str/split.h>
#include <cstdlib>
#include <string>
#include <unordered_map>


class Process;

namespace process_n
{
    class Launcher {
    public:
        Launcher() {}

        friend std::unique_ptr<Launcher> std::make_unique<Launcher>();

        ~Launcher() {
            for (auto& [key, process] : m_processes)
                process.terminate();

            for (auto& [key, process] : m_processes)
                process.wait();
        }

        #ifdef __linux__
            #include <unistd.h>
            #include <sys/wait.h>

            using pipeFds_t = std::array<int, 2>;

            Process& launch(const std::string& executable, std::vector<std::string> stringArgs, bool waitUntilReady=false) {
                stringArgs.insert(stringArgs.begin(), executable);

                pipeFds_t pipeFds;
                if (waitUntilReady) {
                    pipe(pipeFds.data());
                    addReadySequenceArgs(stringArgs, pipeFds);
                }

                std::vector<const char*> args = convertToCharVector(stringArgs);

                processId_t pid = createProcess(args, pipeFds);

                auto [it, _] = m_processes.emplace(pid, pid);
                if (waitUntilReady)
                    if (not waitForReadySequence(pipeFds.data()))
                        throw Exception(std::format("Child process #{}: Failure during ready sequence.", pid));

                return it->second; // Returns reference to emplaced element.
            }
        #endif

        #ifdef _WIN32
            /* TODO */
        #endif
    private:
        std::unordered_map<processId_t, Process> m_processes;

        #ifdef __linux__
            const uint8_t WRITE = 1;
            const uint8_t READ = 0;

            processId_t createProcess(const std::vector<const char*> args, pipeFds_t fds) const {
                processId_t pid = fork();
                if (pid < 0)
                    throw Exception("Error while creating process");

                if (pid == 0) {
                    close(fds[READ]);
                    execvp(args[0], const_cast<char* const*>(args.data()));
                    std::exit(-1);
                }

                return pid;
            }

            void addReadySequenceArgs(std::vector<std::string>& vector, pipeFds_t fds) {
                vector.push_back("--ready-fd");
                vector.push_back(std::to_string(fds[WRITE]));
            }

            std::vector<const char*> convertToCharVector(const std::vector<std::string>& vector) const {
                std::vector<const char*> result(vector.size() + 1);
                for (size_t i = 0; i < vector.size(); i++)
                    result[i] = vector[i].c_str();

                result[result.size() - 1] = nullptr;
                return result;
            }

            bool waitForReadySequence(int* pipeFds) {
                close(pipeFds[WRITE]);

                char buf[6] = {};
                read(pipeFds[READ], buf, 5);

                close(pipeFds[READ]);

                if (strcmp(buf, "READY") == 0)
                    return true;

                return false;
            };
        #endif

        #ifdef _WIN32
            /* TODO */
        #endif
    };
}