#pragma once

#ifdef _WIN32
    #include <core/exception.hpp>
    #include <core/process/process.hpp>
    #include <core/utility/function.hpp>
    #include <string>


    namespace process_n
    {
        class Factory {
        public:
            Factory() = delete;
            ~Factory() = default;

            static Process create(const std::string& executable, std::vector<std::string> stringArgs, bool waitUntilReady);
        };
    }
#endif