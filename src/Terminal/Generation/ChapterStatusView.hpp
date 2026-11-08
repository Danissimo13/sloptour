#pragma once

#include "Terminal/Generation/ChapterStatus.hpp"
#include "Terminal/Localization/Localizer.hpp"
#include "Terminal/Theme/Theme.hpp"

#include <ftxui/dom/elements.hpp>

#include <memory>
#include <utility>

namespace sloptour::terminal {

class ChapterStatusView {
public:
    ChapterStatusView(const Theme& theme, std::shared_ptr<const Localizer> localizer)
        : m_theme(theme)
        , m_localizer(std::move(localizer)) {}

    [[nodiscard]] ftxui::Element icon(const ChapterStatus& status) const;
    [[nodiscard]] ftxui::Element detail(const ChapterStatus& status) const;

    [[nodiscard]] ftxui::Element operator()(const ChapterQueued& status) const;
    [[nodiscard]] ftxui::Element operator()(const ChapterWriting& status) const;
    [[nodiscard]] ftxui::Element operator()(const ChapterWritten& status) const;
    [[nodiscard]] ftxui::Element operator()(const ChapterBroken& status) const;

private:
    Theme m_theme;
    std::shared_ptr<const Localizer> m_localizer;
};

}
