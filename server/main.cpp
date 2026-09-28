
#include <core/http.hpp>
#include <core/logging.hpp>
#include <core/file.hpp>
#include <core/process.hpp>


using commandsMap_t = std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>>;

int main() {
    auto light = log_n::Light::instance();
    const std::vector<std::string> EXTRA_LOGS = { "MAIN" };
    try {
        const std::string BUILD_DIRECTORY = std::format("{}/build", ROOT_DIRECTORY);
        const std::string SERVICE_DIRECTORY = std::format("{}/server/service", ROOT_DIRECTORY);

        std::shared_ptr<file_n::Service> files = file_n::Service::instance();
        const std::vector<std::filesystem::directory_entry>& services = files->entries(SERVICE_DIRECTORY, file_n::flags_n::Entry_en::DIRECTORY);

        std::shared_ptr<process_n::Registry> registry = process_n::Registry::instance();
        std::unordered_map<std::string, processId_t> serviceProcesses;
        for (const auto& dir : services) {
            const std::string& serviceName = dir.path().filename();
            serviceProcesses.emplace(serviceName, registry->start(std::format("{}/server/{}", BUILD_DIRECTORY, serviceName), {}, false).id());
        }

        bool running = true;
        commandsMap_t commands;
        commands["stop"] = [&](const std::vector<std::string>& params) {
            if (not params.empty()) {
                if (not serviceProcesses.contains(params[0]))
                    light->log(log_n::Level_en::WARNING, EXTRA_LOGS, "Service", std::format("\"{}\"", params[0]), "does not exist!");
                else {
                    registry->stop(serviceProcesses[params[0]]);
                    serviceProcesses.erase(params[0]);

                    if (serviceProcesses.empty())
                        running = false;
                }
            }
            else {
                for (const auto& [_, id] : serviceProcesses)
                    registry->stop(id);

                running = false;
            }
        };

        sleep(1);
        while (running) {
            std::string input;
            light->log(log_n::Level_en::INFO, EXTRA_LOGS, "Enter a command:");
            std::getline(std::cin, input);

            std::vector<std::string> tokens = split(input);

            std::string command = tokens[0];
            if (not commands.contains(command)) {
                light->log(log_n::Level_en::WARNING, EXTRA_LOGS, "Command not recognized");
                continue;
            }

            tokens.erase(tokens.begin());
            commands[command](tokens);
        }
        return 0;
    } catch (Exception e) {
        light->log(log_n::Level_en::ERROR, EXTRA_LOGS, "Uncaught error: ", e);
    }
}