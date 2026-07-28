#include <core/file/file.hpp>


File::File(std::string_view path, std::ios_base::openmode flags) : m_flags(flags), m_path(path) {
    m_file = std::make_unique<std::fstream>(m_path, flags);

    if (not m_file or
        not m_file->is_open())
        throw Exception("Could not open the given file");
}

File::~File() {
    m_file->close();
}

File& File::operator<<(std::string_view data) {
    *m_file << std::format("{} ", data);
    return *this;
}

const File& File::operator>>(std::string& data) const {
    std::string extracted;
    *m_file >> extracted;
    data.append(std::format("{} ", extracted));
    return *this;
}

std::ios_base::openmode File::flagToUnderlying(file_n::flags_n::OpenMode_en flag) {
    return static_cast<std::ios_base::openmode>(flag);
}

bool File::isFlagSet(file_n::flags_n::OpenMode_en flag) const {
    return m_flags & flagToUnderlying(flag);
}

bool File::isPointerPositive(std::streampos pointer) const {
    return pointer >= 0;
}

std::streampos File::pointer() const {
    return m_file->tellg();
}

std::string File::read(std::streampos start, std::streampos end) const {
    if(not isFlagSet(file_n::flags_n::OpenMode_en::READ))
       throw InvalidArgument("Cannot read because READ flag has not been set", "File flags");

    if (not isPointerPositive(start) or
        not isPointerPositive(end) or
        end <= start)
        throw InvalidArgument("\"start\" and \"end\" pointers must be positive, with \"end\" being greater than \"start\"", "Read file pointers");

    std::streampos size = std::streampos(end - start);

    m_file->seekg(start);

    std::string file(size, '\0');
    m_file->read(&file[0], size);

    if (not m_file->eof() and m_file->fail())
        throw Exception(std::format("Error while reading file at {}", m_path));

    return file;
}

std::string File::read(std::streampos start) const {
    return read(isPointerPositive(start) ? start : std::streampos(0), this->size());
}

void File::setPointer(std::streampos value) {
    m_file->seekg(value);
}

std::streampos File::size() const {
    m_file->seekg(0, std::fstream::end); // Go to end of file.
    std::streampos size = m_file->tellg();
    return size;
}

size_t File::write(std::string_view data, std::streampos start) {
    if(not isFlagSet(file_n::flags_n::OpenMode_en::WRITE))
       throw InvalidArgument("Cannot write because WRITE flag has not been set", "File flags");

    if (not isPointerPositive(start))
        start = 0;

    m_file->seekg(start);

    size_t size = data.size();
    m_file->write(data.data(), size);

    return size;
}