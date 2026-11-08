#pragma once

#include "Application/Ports/PortErrors.hpp"
#include "Domain/Progress/TourProgress.hpp"
#include "Domain/Tour/StepPosition.hpp"
#include "Domain/Tour/Tour.hpp"
#include "Terminal/Localization/Localizer.hpp"
#include "Terminal/Markdown/MarkdownView.hpp"
#include "Terminal/Player/CodeExcerptView.hpp"
#include "Terminal/Theme/Theme.hpp"

#include <ftxui/dom/elements.hpp>

#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace sloptour::terminal {

using SourceLookup =
    std::function<const application::ApplicationResult<std::vector<std::string>>&(std::string_view path)>;

class StepView {
public:
    StepView(Theme theme, std::shared_ptr<const MarkdownView> markdown, CodeExcerptView code,
             std::shared_ptr<const Localizer> text)
        : m_theme(theme)
        , m_markdown(std::move(markdown))
        , m_code(std::move(code))
        , m_text(std::move(text)) {}

    [[nodiscard]] ftxui::Element render(const domain::Tour& tour, const domain::TourProgress& progress,
                                        domain::StepPosition position, std::size_t location,
                                        const SourceLookup& sources) const;

private:
    [[nodiscard]] ftxui::Element heading(const domain::Tour& tour, domain::StepPosition position) const;
    [[nodiscard]] ftxui::Element commands(const domain::Step& step) const;
    [[nodiscard]] ftxui::Element checkpoint(const domain::Checkpoint& checkpoint, bool revealed) const;

    Theme m_theme;
    std::shared_ptr<const MarkdownView> m_markdown;
    CodeExcerptView m_code;
    std::shared_ptr<const Localizer> m_text;
};

}
