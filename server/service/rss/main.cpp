#include <service/rss.hpp>


int main() {
    auto light = log_n::Light::instance();
    try {
        const nlohmann::json& configJSON = readConfigJSON(std::format("{}/settings/launch.json", SERVICE_DIRECTORY));
        std::unique_ptr<RSSService> server = RSSService::create(configJSON);

        server->setDatabase(PostgreSQL::instance(configJSON["postgres"]));
        server->setCache(Redis::instance(configJSON["redis"]));

        server->run();

        return 0;
    } catch (Exception e) {
        light->log(log_n::Level_en::ERROR,  "Uncaught error: ", e);
    }
    return 0;
}