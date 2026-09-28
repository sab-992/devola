#include <core/process/process.hpp>


#ifdef __linux__
    Process::Process(processId_t pid) : m_pid(pid), m_status(process_n::Status_en::RUNNING) {}

    Process::~Process() {
        if (not m_pipe.empty())
            unlink(m_pipe.c_str());
    }

    processId_t Process::id() const {
        return m_pid;
    }

    void Process::pipe(std::string_view path) {
        if (not m_pipe.empty()) return;
        m_pipe = std::format("/tmp/{}", path);
        const mode_t MODE = 0666;
        ::mkfifo(m_pipe.c_str(), MODE);
    }

    void Process::receive(std::string& content) {
        std::ifstream pipe(m_pipe);
        std::stringstream ss;
        ss << pipe.rdbuf();
        content = ss.str();
    }

    void Process::send(std::string_view content) {
        if (m_pipe.empty())
            throw Exception(std::format("PID: {} => Impossible to send data, no pipe created.", m_pid));

        File pipe = file_n::Service::instance()->open(m_pipe, file_n::flags_n::OpenMode_en::WRITE);
        pipe.write(content);
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
        ::waitpid(m_pid, &status, 0);
        m_status = process_n::Status_en::STOPPED;
        return WEXITSTATUS(status);
    }
#endif