#include <core/file/service.hpp>


file_n::Service::Service(const Private_s&) {}

file_n::Service::~Service() {}

bool file_n::Service::createDirectories(std::string_view path) {
    return std::filesystem::create_directories(path);
}