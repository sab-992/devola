#include <core/file/service.hpp>


file_n::Service::Service(const Private_s&) {}

file_n::Service::~Service() {}

bool file_n::Service::createDirectories(std::string_view path) const {
    return std::filesystem::create_directories(path);
}

bool file_n::Service::isEntryFlagSet(uint16_t flags, file_n::flags_n::Entry_en flag) const {
    return flags & to_underlying(flag);
}

std::vector<std::filesystem::directory_entry> file_n::Service::retrieve_entries(std::string_view path, uint16_t filters) const {
    using flags_n::Entry_en;

    std::filesystem::path directory(path);
    std::vector<std::filesystem::directory_entry> entries;
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (isEntryFlagSet(filters, Entry_en::FILE) and entry.is_regular_file())
            entries.emplace_back(entry);
        else if (isEntryFlagSet(filters, Entry_en::DIRECTORY) and entry.is_directory())
            entries.emplace_back(entry);
    }

    return entries;
}