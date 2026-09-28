#include <core/process/registry.hpp>


process_n::Registry::Registry(const Private_s&) {}

process_n::Registry::~Registry() {
    for (auto& [key, process] : m_processes)
        process.terminate();

    for (auto& [key, process] : m_processes)
        process.wait();

    m_processes.clear();
}

void process_n::Registry::send(processId_t identifier, std::string_view pipe, std::string_view content) {
    validateProcessExists(identifier);
    Process& process = m_processes.at(identifier);

    process.pipe(pipe);
    process.send(content);
}

std::string process_n::Registry::receive(processId_t identifier, std::string_view pipe) {
    validateProcessExists(identifier);

    Process& process = m_processes.at(identifier);

    process.pipe(pipe);

    std::string data;
    process.receive(data);
    return data;
}

const Process& process_n::Registry::start(const std::string &executable, std::vector<std::string> stringArgs, bool waitUntilReady) {
    Process process = Factory::create(executable, stringArgs, waitUntilReady);
    const auto& [it, _] = m_processes.emplace(process.id(), std::move(process));
    return it->second;
}

void process_n::Registry::stop(processId_t identifier) {
    validateProcessExists(identifier);

    Process& process = m_processes.at(identifier);

    process.terminate();
    process.wait();

    m_processes.erase(process.id());
    rehashIfNeeded(m_processes);
}

void process_n::Registry::validateProcessExists(processId_t identifier) {
    if (not m_processes.contains(identifier))
        throw InvalidArgument("Does not exist", "Process identifier");
}