#pragma once

#include <ftxui/screen/color.hpp>

namespace sloptour::terminal {

struct Theme {
    ftxui::Color background;
    ftxui::Color badge_text;
    ftxui::Color accent;
    ftxui::Color accent_soft;
    ftxui::Color text;
    ftxui::Color muted;
    ftxui::Color success;
    ftxui::Color warning;
    ftxui::Color danger;
    ftxui::Color code_background;
    ftxui::Color code_focus;
    ftxui::Color keyword;
    ftxui::Color string;
    ftxui::Color number;
    ftxui::Color comment;
    ftxui::Color heading;
    ftxui::Color inline_code;
};

}
