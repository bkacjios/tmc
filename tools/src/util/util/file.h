#ifndef TOOLS_FILE_H
#define TOOLS_FILE_H

#include <cstdio>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>

namespace util {
namespace {
auto file_ptr_deleter = [](std::FILE* file) { std::fclose(file); };
}

using file_ptr_t = std::unique_ptr<std::FILE, decltype(file_ptr_deleter)>;

inline file_ptr_t open_file(std::string_view filename, std::string_view mode) {
    return { std::fopen(filename.data(), mode.data()), file_ptr_deleter };
}

inline file_ptr_t open_file(const std::string& filename, std::string_view mode) {
    return { std::fopen(filename.c_str(), mode.data()), file_ptr_deleter };
}

// A path does not convert to std::string implicitly on Windows.
inline file_ptr_t open_file(const std::filesystem::path& filename, std::string_view mode) {
    return open_file(filename.string(), mode);
}

} // namespace util
#endif // TOOLS_FILE_H
