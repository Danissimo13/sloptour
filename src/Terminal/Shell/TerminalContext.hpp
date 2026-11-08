#pragma once

#include "Terminal/Code/SyntaxHighlighter.hpp"
#include "Terminal/Localization/Localizer.hpp"
#include "Terminal/Markdown/MarkdownView.hpp"
#include "Terminal/Settings/TerminalSettings.hpp"
#include "Terminal/Shell/ScreenControls.hpp"
#include "Terminal/Shell/TerminalServices.hpp"
#include "Terminal/Theme/Theme.hpp"

#include <memory>

namespace sloptour::terminal {

struct TerminalContext {
    ScreenControls controls;
    TerminalServices services;
    TerminalSettings settings;
    Theme theme;
    std::shared_ptr<const SyntaxHighlighter> highlighter;
    std::shared_ptr<const MarkdownView> markdown;
    std::shared_ptr<Localizer> localizer;
};

}
