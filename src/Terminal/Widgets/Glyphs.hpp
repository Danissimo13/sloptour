#pragma once

#include <string_view>

namespace sloptour::terminal::glyphs {

constexpr std::string_view LOGO = "◆";
constexpr std::string_view DONE = "✓";
constexpr std::string_view CURRENT = "●";
constexpr std::string_view PENDING = "○";
constexpr std::string_view FAILED = "✗";
constexpr std::string_view POINTER = "▸";
constexpr std::string_view PROMPT = "❯";
constexpr std::string_view QUESTION = "?";
constexpr std::string_view RUN = "$";
constexpr std::string_view SEPARATOR = " · ";
constexpr std::string_view ELLIPSIS = "…";
constexpr std::string_view ADD = "+";
constexpr std::string_view STAR = "★";

}
