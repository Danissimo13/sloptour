#pragma once

#include "Terminal/Shell/Destination.hpp"
#include "Terminal/Shell/TerminalContext.hpp"

#include <memory>
#include <utility>

namespace sloptour::terminal {

class Navigator {
public:
    explicit Navigator(std::shared_ptr<const TerminalContext> context)
        : m_context(std::move(context)) {}

    [[nodiscard]] Destination operator()(const GoHome& destination) const;
    [[nodiscard]] Destination operator()(const GoWizard& destination) const;
    [[nodiscard]] Destination operator()(const GoGenerate& destination) const;
    [[nodiscard]] Destination operator()(const GoPlay& destination) const;
    [[nodiscard]] Destination operator()(const GoExit& destination) const;

private:
    std::shared_ptr<const TerminalContext> m_context;
};

}
