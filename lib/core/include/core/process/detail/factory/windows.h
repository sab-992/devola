#pragma once

#ifdef _WIN32
    #include <core/exception.h>
    #include <core/process/process.h>
    #include <core/utility/function.h>
    #include <string>


    // TODO: Complete class
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