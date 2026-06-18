#pragma once

#include <core/exception.hpp>
#include <core/file/detail/type.hpp>
#include <fstream>
#include <memory>
#include <string>


class File {
public:
    File(const std::string& path, std::ios_base::openmode flags);
    ~File();

    File& operator<<(std::string_view data);
    File& operator>>(std::string& buffer);

    std::streampos pointer();

    // Read from start to end (included) position => [start, end].
    std::string read(std::streampos start, std::streampos end);

    // Read all from start position.
    std::string read(std::streampos start=-1);

    void setPointer(std::streampos value);

    size_t write(std::string_view data, std::streampos start=-1);

    static std::ios_base::openmode flagToUnderlying(file_n::Flags_en flag);

private:
    std::unique_ptr<std::fstream> m_file;
    std::ios_base::openmode m_flags;
    const std::string m_path;

    bool isFlagSet(file_n::Flags_en flag) const;
    bool isPointerPositive(std::streampos pointer) const;

    std::streampos size() const;
};