#include "util.h"
#include <filesystem>
#include <iostream>
#include <fmt/format.h>

void check_call(const std::vector<std::string>& cmd) {
    std::string cmdstr;
    bool first = true;
    for (const auto& segment : cmd) {
        if (first) {
            first = false;
        } else {
            cmdstr += " ";
        }
        cmdstr += segment;
    }
    int code = system(cmdstr.c_str());
    if (code != 0) {
        std::cerr << cmdstr << " failed with return code " << code << std::endl;
        std::exit(1);
    }
}

std::string tool_path(const std::string& name) {
    // TOOLS_BIN_DIR is relative to the repository root (CMakeLists.txt). In the
    // native separators: on Windows, system() runs cmd.exe, which takes a '/'
    // in the command name for an option.
    return (std::filesystem::path(TOOLS_BIN_DIR) / name).make_preferred().string();
}

std::string opt_param(const std::string& format, int defaultVal, int value) {
    if (value != defaultVal) {
        return fmt::format(format, value);
    }
    return "";
}
