#include <core/process/process.h>

#ifdef _WIN32
    Process::Process(processId_t pid) : m_pid(pid), m_status(process_n::Status_en::RUNNING) {}

    processId_t Process::id() const {
        return m_pid;
    }

    process_n::Status_en Process::state() const {
        return m_status;
    }

    void Process::terminate() {
        throw NotSupported(FUNCTION_SIGNATURE);
    }

    int Process::wait() {
        throw NotSupported(FUNCTION_SIGNATURE);
    }
#endif