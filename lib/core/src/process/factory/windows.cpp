#include <core/process/detail/factory/windows.hpp>

#ifdef _WIN32
    Process process_n::Factory::create(const std::string& executable, std::vector<std::string> stringArgs, bool waitUntilReady) {
        throw NotSupported(FUNCTION_SIGNATURE);
    }
#endif