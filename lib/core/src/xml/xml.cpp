#include <core/xml/xml.h>

xml_n::Document::Document(const std::string& document) {
    if (not document.empty())
        this->load_string(document.c_str());
}

bool xml_n::Document::operator==(const Document& other) {
    std::ostringstream currentSS, otherSS;
    this->print(currentSS, "", pugi::format_raw);
    other.print(otherSS, "", pugi::format_raw);
    return currentSS.str() == otherSS.str();
}

bool xml_n::Document::operator!=(const Document& other) {
    return !(*this == other);
}

std::string xml_n::Document::toString() const {
    std::ostringstream ss;
    this->print(ss);
    return ss.str();
}