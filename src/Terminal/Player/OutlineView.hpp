#pragma once

#include "Domain/Progress/TourProgress.hpp"
#include "Domain/Tour/StepPosition.hpp"
#include "Domain/Tour/Tour.hpp"
#include "Terminal/Localization/Localizer.hpp"
#include "Terminal/Theme/Theme.hpp"

#include <ftxui/dom/elements.hpp>

#include <cstddef>
#include <memory>
#include <optional>
#include <utility>

namespace sloptour::terminal {

class OutlineView {
public:
    OutlineView(Theme theme, std::size_t width, std::shared_ptr<const Localizer> text)
        : m_theme(theme)
        , m_width(width)
        , m_text(std::move(text)) {}

    [[nodiscard]] ftxui::Element render(const domain::Tour& tour, const domain::TourProgress& progress,
                                        std::optional<domain::StepPosition> current) const;

private:
    [[nodiscard]] ftxui::Element chapter(const domain::Tour& tour, const domain::TourProgress& progress,
                                         std::size_t index, std::optional<domain::StepPosition> current) const;
    [[nodiscard]] ftxui::Element quiz(const domain::Tour& tour, const domain::TourProgress& progress,
                                      bool active) const;
    [[nodiscard]] ftxui::Element step(const domain::Step& step, bool visited, bool active) const;
    [[nodiscard]] ftxui::Color title_color(bool complete, bool active) const;

    Theme m_theme;
    std::size_t m_width;
    std::shared_ptr<const Localizer> m_text;
};

}
