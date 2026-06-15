#include <core/process/registry.h>


process_n::Registry::Registry(const Private_s&) {}

process_n::Registry::~Registry() {
    for (auto& [key, process] : m_processes)
        process.terminate();

    for (auto& [key, process] : m_processes)
        process.wait();

    m_processes.clear();
}

const Process& process_n::Registry::start(const std::string &executable, std::vector<std::string> stringArgs, bool waitUntilReady) {
    Process process = Factory::create(executable, stringArgs, waitUntilReady);
    const auto& [it, _] = m_processes.emplace(process.id(), std::move(process));
    return it->second;
}

void process_n::Registry::stop(processId_t identifier) {
    if (not m_processes.contains(identifier))
        throw InvalidArgument("Does not exist", "Process identifier");

    Process& process = m_processes.at(identifier);

    process.terminate();
    process.wait();

    m_processes.erase(process.id());
    rehashIfNeeded(m_processes);
}