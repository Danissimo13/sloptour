#pragma once

#include "Terminal/Code/LanguageSyntax.hpp"
#include "Terminal/Localization/LanguagePack.hpp"
#include "Terminal/Theme/ThemePalette.hpp"

#include <cstddef>
#include <vector>

namespace sloptour::terminal {

struct TerminalSettings {
    std::size_t outline_width = 0;
    std::size_t code_context_lines = 0;
    std::size_t code_max_lines = 0;
    std::size_t activity_lines = 0;
    std::size_t tab_width = 0;
    std::size_t chat_height = 0;
    std::size_t recent_commits = 0;
    ThemePalette palette;
    std::vector<LanguageSyntax> languages;
    std::vector<LanguagePack> packs;
};

}
