#pragma once


#ifdef __linux__
    #include <unistd.h>
    #include <sys/wait.h>
    #include <core/process/detail/status.h>
    #include <core/process/registry.h>


    using processId_t = pid_t;

    class Process {
    public:
        explicit Process(pid_t pid) : m_pid(pid), m_status(process_n::Status_en::RUNNING) {}
        ~Process() = default;

        Process(const Process&) = default;

        processId_t id() const { return m_pid; }
        bool valid() const { return m_pid > 0; }

        int wait() {
            int status = 0;
            waitpid(m_pid, &status, 0);
            m_status = process_n::Status_en::TERMINATED;
            return WEXITSTATUS(status);
        }

        void terminate() {
            if (m_status != process_n::Status_en::RUNNING) return;

            ::kill(m_pid, SIGTERM);
            m_status = process_n::Status_en::TERMINATED;
        }

    private:
        process_n::Status_en m_status;
        pid_t m_pid;
    };
#endif