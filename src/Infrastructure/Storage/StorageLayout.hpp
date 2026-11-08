#pragma once

#include <string_view>

namespace sloptour::infrastructure::storage_layout {

constexpr std::string_view TOURS = "tours";
constexpr std::string_view PROGRESS = "progress";
constexpr std::string_view PROMPTS = "prompts";
constexpr std::string_view CONFIG = "config.json";
constexpr std::string_view PREFERENCES = "preferences.json";
constexpr std::string_view DOCUMENT_EXTENSION = ".json";
constexpr int JSON_INDENT = 2;

}
