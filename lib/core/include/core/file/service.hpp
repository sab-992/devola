#pragma once

#include <core/utility/enum.hpp>
#include <core/file/detail/entries.hpp>
#include <core/file/detail/flags.hpp>
#include <core/file/detail/status.hpp>
#include <core/file/file.hpp>
#include <core/utility/singleton.hpp>


namespace file_n
{
    class Service : public Singleton<Service> {
    public:
        Service(const Private_s&);
        ~Service();

        template <typename... Args>
            requires (std::is_convertible_v<Args, file_n::flags_n::OpenMode_en> && ...)
        File open(const std::string& path, Args... flags) const {
            return File(path, (File::flagToUnderlying(flags) | ...));
        }

        template <typename... Args>
            requires (std::is_convertible_v<Args, file_n::flags_n::Entry_en> && ...)
        std::vector<std::filesystem::directory_entry> entries(std::string_view path, Args... filters) const {
            if (not std::filesystem::exists(path))
                throw InvalidArgument(std::format("Path: '{}' does not exist !", path));

            return retrieve_entries(path, (to_underlying(filters) | ...));
        }

        bool createDirectories(std::string_view path) const;
    private:
        bool isEntryFlagSet(uint16_t flags, file_n::flags_n::Entry_en flag) const;

        std::vector<std::filesystem::directory_entry> retrieve_entries(std::string_view path, uint16_t filters) const;
    };
}