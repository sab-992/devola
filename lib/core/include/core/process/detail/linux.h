#pragma once


#ifdef __linux__
    #include <unistd.h>
    #include <sys/wait.h>
    #include <core/process/detail/status.h>
    #include <core/process/launcher.h>


    using processId_t = pid_t;

    class Process {
    public:
        explicit Process(pid_t pid) : m_pid(pid), m_status(Status_en::RUNNING) {}
        ~Process() = default;

        friend class Launcher;

        processId_t id() const { return m_pid; }
        bool valid() const { return m_pid > 0; }

        int wait() {
            if (m_status != Status_en::RUNNING) return -1;

            int status = 0;
            waitpid(m_pid, &status, 0);
            m_status = Status_en::TERMINATED;
            std::cout << "done after waiting: " << m_pid << std::endl;
            return WEXITSTATUS(status);
        }

        void terminate() {
            if (m_status != Status_en::RUNNING) return;

            ::kill(m_pid, SIGTERM);
            m_status = Status_en::TERMINATED;
            std::cout << "terminated: " << m_pid << std::endl;
        }

    private:
        Status_en m_status;
        pid_t m_pid;
    };
#endif