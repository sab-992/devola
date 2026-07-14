
#include <core/logging.hpp>
#include <core/file.hpp>
#include <core/process.hpp>


using commandsMap_t = std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>>;


int main() {
    auto light = log_n::Light::instance();
    try {
        const std::string BUILD_DIRECTORY = std::format("{}/build", ROOT_DIRECTORY);
        const std::string SERVICE_DIRECTORY = std::format("{}/server/service", ROOT_DIRECTORY);

        std::shared_ptr<file_n::Service> files = file_n::Service::instance();
        const std::vector<std::filesystem::directory_entry>& services = files->entries(SERVICE_DIRECTORY, file_n::flags_n::Entry_en::DIRECTORY);

        std::shared_ptr<process_n::Registry> registry = process_n::Registry::instance();
        std::vector<processId_t> processes;
        for (const auto& dir : services)
            processes.emplace_back(registry->start(std::format("{}/server/{}", BUILD_DIRECTORY, std::string(dir.path().filename())), {}, false).id());

        bool running = true;
        commandsMap_t commands;
        commands["stop"] = [&](const std::vector<std::string>& params) {
            for (auto id : processes)
                registry->stop(id);

            running = false;
        };

        light->log(log_n::Level_en::INFO, "Reading user input...");
        while (running) {
            std::string input;
            std::getline(std::cin, input);

            std::vector<std::string> tokens = split(input);

            std::string command = tokens[0];
            if (not commands.contains(command)) {
                light->log(log_n::Level_en::WARNING, "Command not recognized");
                continue;
            }

            tokens.erase(tokens.begin());
            commands[command](tokens);
        }
        return 0;
    } catch (std::exception e) {
        light->log(log_n::Level_en::ERROR, "Uncaught error: ", e.what());
    }
}