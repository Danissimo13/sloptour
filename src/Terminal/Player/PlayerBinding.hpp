#pragma once

#include <ftxui/component/event.hpp>

#include <functional>
#include <string_view>
#include <vector>

namespace sloptour::terminal {

struct PlayerBinding {
    std::vector<ftxui::Event> events;
    std::string_view keys;
    std::string_view description;
    std::function<void()> action;
};

}
