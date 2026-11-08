#pragma once

#include "Terminal/Settings/TerminalSettings.hpp"
#include "Terminal/Shell/Destination.hpp"
#include "Terminal/Shell/TerminalServices.hpp"

#include <utility>

namespace sloptour::terminal {

class TerminalApp {
public:
    TerminalApp(TerminalServices services, TerminalSettings settings)
        : m_services(std::move(services))
        , m_settings(std::move(settings)) {}

    void run(Destination first) const;

private:
    TerminalServices m_services;
    TerminalSettings m_settings;
};

}
