#include <core/thread/registry.hpp>


thread_n::Registry::Registry(const Private_s&) {}

thread_n::Registry::~Registry() {
    std::vector<std::thread::id> threads;

    for (auto& [id, _] : m_threads)
        threads.emplace_back(id);

    for (auto& id : threads)
        join(id);

    m_threads.clear();
}

const Thread& thread_n::Registry::start(thread_n::completionToken_t function) {
    Thread thread = thread_n::Factory::create(function);
    const auto& [it, _] = m_threads.emplace(thread.id(), std::move(thread));
    return it->second;
}

void thread_n::Registry::join(std::thread::id id) {
    // This cannot throw an error because it is possible that the caller is not the
    // threadRegistry, in which case when the Registry's destructor is called, it will
    // throw an error cause the thread would not exist.
    if (not m_threads.contains(id))
        return;

    if (m_threads.at(id).status() != thread_n::Status_en::RUNNING)
        throw LogicException("Non-running thread still exists in the registry");

    m_threads.erase(id);
    rehashIfNeeded(m_threads);
}