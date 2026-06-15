#pragma once

#include <core/process/detail/process/linux.hpp>
#include <core/process/detail/process/windows.hpp>
#include <core/process/detail/status.hpp>


class Process {
public:
    explicit Process(processId_t pid);
    Process(const Process&) = default;
    ~Process() = default;

    processId_t id() const;
    process_n::Status_en state() const;

    void terminate();
    int wait();

private:
    process_n::Status_en m_status;
    processId_t m_pid;
};