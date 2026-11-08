#pragma once

#include "Domain/Progress/Exchange.hpp"
#include "Terminal/Localization/Localizer.hpp"
#include "Terminal/Markdown/MarkdownView.hpp"
#include "Terminal/Player/RunningTask.hpp"
#include "Terminal/Theme/Theme.hpp"

#include <ftxui/dom/elements.hpp>

#include <memory>
#include <optional>
#include <span>
#include <string_view>
#include <utility>

namespace sloptour::terminal {

class ChatPanel {
public:
    ChatPanel(Theme theme, std::shared_ptr<const MarkdownView> markdown, std::shared_ptr<const Localizer> text)
        : m_theme(theme)
        , m_markdown(std::move(markdown))
        , m_text(std::move(text)) {}

    [[nodiscard]] ftxui::Element render(std::span<const domain::Exchange> history,
                                        const std::optional<RunningTask>& pending, ftxui::Element input, bool typing,
                                        std::string_view agent, float scroll) const;

private:
    [[nodiscard]] ftxui::Element question(std::string_view text) const;
    [[nodiscard]] ftxui::Element waiting(const RunningTask& task) const;
    [[nodiscard]] ftxui::Element input_row(ftxui::Element input, bool typing, bool busy) const;

    Theme m_theme;
    std::shared_ptr<const MarkdownView> m_markdown;
    std::shared_ptr<const Localizer> m_text;
};

}
