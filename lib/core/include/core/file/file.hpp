#pragma once

#include <core/exception.hpp>
#include <core/file/detail/flags.hpp>
#include <fstream>
#include <memory>
#include <string>


class File {
public:
    File(std::string_view path, std::ios_base::openmode flags);
    ~File();

    File& operator<<(std::string_view data);
    const File& operator>>(std::string& buffer) const;

    std::streampos pointer() const;

    // Read from start to end (included) position => [start, end].
    std::string read(std::streampos start, std::streampos end) const;

    // Read all from start position.
    std::string read(std::streampos start=-1) const;

    void setPointer(std::streampos value);

    size_t write(std::string_view data, std::streampos start=-1);

    static std::ios_base::openmode flagToUnderlying(file_n::flags_n::OpenMode_en flag);

private:
    std::unique_ptr<std::fstream> m_file;
    std::ios_base::openmode m_flags;
    const std::string m_path;

    bool isFlagSet(file_n::flags_n::OpenMode_en flag) const;
    bool isPointerPositive(std::streampos pointer) const;

    std::streampos size() const;
};