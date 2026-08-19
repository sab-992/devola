#include <core/http/detail/routes.hpp>


void http_n::server_n::RadixRouter::addRoute(std::string_view routeTemplate, handler_t handler) {
    const auto& segments = tokenize(routeTemplate);
    auto current = m_root;

    for (auto seg : segments) {
        if (seg.front() == '{' and seg.back() == '}') {
            std::string paramName = std::string(seg.substr(1, seg.length() - 2));

            if (not current->paramChild) {
                current->paramChild = std::make_shared<Node>();
                current->paramChild->isParam = true;
                current->paramChild->paramName = paramName;
            }

            current = current->paramChild;
        }
        else {
            std::string key(seg);
            if (current->staticChildren.find(key) == current->staticChildren.end()) {
                auto newNode = std::make_shared<Node>();
                newNode->segment = key;
                current->staticChildren[key] = newNode;
            }

            current = current->staticChildren[key];
        }
    }

    current->handler = handler;
}

boundHandler_t http_n::server_n::RadixRouter::route(std::string_view requestPath) const {
    const auto& segments = tokenize(requestPath);
    pathParams_t params;

    auto current = m_root;
    for (auto seg : segments) {
        std::string key(seg);

        if (auto it = current->staticChildren.find(key); it != current->staticChildren.end())
            current = it->second;
        else if (current->paramChild) {
            current = current->paramChild;
            params[current->paramName] = key;
        }
        else
            return nullptr;
    }

    if (not current or not current->handler)
        return nullptr;

    // TODO: add query parameters
    return std::bind(current->handler, std::placeholders::_1, std::placeholders::_2, params);
}

std::vector<std::string_view> http_n::server_n::RadixRouter::tokenize(std::string_view path) const {
    std::vector<std::string_view> segments;
    size_t start = 0;

    while (start < path.length()) {
        size_t end = path.find('/', start);
        if (end == std::string_view::npos)
            end = path.length();
        if (end > start)
            segments.push_back(path.substr(start, end - start));
        start = end + 1;
    }

    return segments;
}