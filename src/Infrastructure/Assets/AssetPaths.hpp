#pragma once

#include <string_view>

namespace sloptour::infrastructure::asset_paths {

constexpr std::string_view DEFAULT_CONFIG = "config/defaults.json";
constexpr std::string_view SYNTAX = "syntax/languages.json";
constexpr std::string_view PROMPTS_DIRECTORY = "prompts/";
constexpr std::string_view PROMPT_EXTENSION = ".md";

}
