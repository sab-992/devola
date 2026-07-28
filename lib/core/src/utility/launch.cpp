#include <core/utility/launch.hpp>


nlohmann::json readConfigJSON(std::string_view path) {
    auto files = file_n::Service::instance();
    auto launchFile = files->open(path, file_n::flags_n::OpenMode_en::READ);

    nlohmann::json JSON = nlohmann::json::parse(launchFile.read())["configuration"];

    #ifdef DEBUG_MODE_ENABLED
        return JSON["debug"];
    #else
        return JSON["production"];
    #endif
}
