#pragma once

#include <ftxui/component/component_base.hpp>

#include <functional>

namespace sloptour::terminal {

struct ScreenControls {
    std::function<void(ftxui::Component)> loop;
    std::function<void()> exit;
    std::function<void(std::function<void()>)> post;
    std::function<void()> redraw;
    std::function<void(std::function<void()>)> suspended;
};

}
