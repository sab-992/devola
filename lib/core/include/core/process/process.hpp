#pragma once

#include <core/file/service.hpp>
#include <core/process/detail/process/linux.hpp>
#include <core/process/detail/process/windows.hpp>
#include <core/process/detail/status.hpp>
#include <fcntl.h>
#include <format>
#include <sys/stat.h>
#include <string>


class Process {
public:
    explicit Process(processId_t pid);
    Process(const Process&) = default;
    ~Process();

    processId_t id() const;
    void pipe(std::string_view path);
    void send(std::string_view content);
    void receive(std::string& content);
    process_n::Status_en state() const;

    void terminate();
    int wait();

private:
    processId_t m_pid;
    std::string m_pipe;
    process_n::Status_en m_status;
};