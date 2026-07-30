#include <service/rss.hpp>


RSS::RSS(const Private_s&, const json& configJSON) : m_configJSON(configJSON), Basic("RSS", configJSON["server"]["port"]) {
    setStartSequence([&](Basic*){
        assert(this->m_database && "[RSS]: No database given");
    });
    setEndpoints();
}

RSS::~RSS() {}

std::unique_ptr<RSS> RSS::create(const json& configJSON) {
    return std::make_unique<RSS>(Private_s(), configJSON);
}

std::string RSS::pathPrefix() const {
    return "/rss";
}

void RSS::setDatabase(std::shared_ptr<Database_i> database) {
    m_database = database;
}

void RSS::setEndpoints() {}