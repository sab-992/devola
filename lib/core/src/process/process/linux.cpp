#include <core/process/process.h>

#ifdef __linux__
    Process::Process(processId_t pid) : m_pid(pid), m_status(process_n::Status_en::RUNNING) {}

    processId_t Process::id() const {
        return m_pid;
    }

    process_n::Status_en Process::state() const {
        return m_status;
    }

    void Process::terminate() {
        if (m_status != process_n::Status_en::RUNNING) return;

        ::kill(m_pid, SIGTERM);
        m_status = process_n::Status_en::TERMINATING;
    }

    int Process::wait() {
        int status = 0;
        waitpid(m_pid, &status, 0);
        m_status = process_n::Status_en::STOPPED;
        return WEXITSTATUS(status);
    }
#endif