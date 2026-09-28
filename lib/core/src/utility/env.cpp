#include <core/utility/env.hpp>


env_t env(std::string_view path) {
    std::unordered_map<std::string, std::string> envVariables;

    auto files = file_n::Service::instance();
    File env = files->open(path, file_n::flags_n::OpenMode_en::READ);

    std::string line;
    std::stringstream stream = std::stringstream{ env.read() };
    while (std::getline(stream, line)) {
        trim(line);

        if (line.empty() || line[0] == '#')
            continue;

        const std::vector<std::string>& keyval = split(line, "=");
        if (keyval.size() <= 1)
            continue;

        std::string key = trim(keyval[0]);
        std::string value = trim(keyval[1]);

        envVariables[key] = value;
    }

    return envVariables;
}