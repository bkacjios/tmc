#ifndef UTIL_H
#define UTIL_H

#include <nlohmann/json_fwd.hpp>
#include <memory>
#include <stdexcept>
#include <cstdlib>
#include <string>
#include <vector>

void check_call(const std::vector<std::string>& cmd);

// Path to one of the other tools, as installed next to asset_processor.
std::string tool_path(const std::string& name);

std::string opt_param(const std::string& format, int defaultVal, int value);

#endif