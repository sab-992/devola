#include <service/user.hpp>


int main() {
    auto light = log_n::Light::instance();
    try {
        const nlohmann::json& configJSON = readConfigJSON(std::format("{}/settings/launch.json", SERVICE_DIRECTORY));
        std::unique_ptr<UserService> server = UserService::create(configJSON);

        server->setDatabase(PostgreSQL::instance(configJSON["postgres"]));

        server->run();

        return 0;
    } catch (Exception e) {
        light->log(log_n::Level_en::ERROR,  "Uncaught error: ", e);
    }
    return 0;
}