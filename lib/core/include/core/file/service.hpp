#pragma once

#include <core/utility/enum.hpp>
#include <core/file/detail/status.hpp>
#include <core/file/detail/type.hpp>
#include <core/file/file.hpp>
#include <core/utility/singleton.hpp>


namespace file_n
{
    class Service : public Singleton<Service> {
    public:
        Service(const Private_s&);
        ~Service();

        template <typename... Args>
            requires (std::is_convertible_v<Args, file_n::Flags_en> && ...)
        File open(const std::string& path, Args... flags) {
            return File(path, (File::flagToUnderlying(flags) | ...));
        }
    };
}