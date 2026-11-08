#pragma once

#include "Domain/Progress/TourProgress.hpp"
#include "Domain/Quiz/Quiz.hpp"
#include "Terminal/Localization/Localizer.hpp"
#include "Terminal/Markdown/MarkdownView.hpp"
#include "Terminal/Theme/Theme.hpp"

#include <ftxui/dom/elements.hpp>

#include <cstddef>
#include <memory>
#include <optional>
#include <string_view>
#include <utility>

namespace sloptour::terminal {

class QuizPanel {
public:
    QuizPanel(Theme theme, std::shared_ptr<const MarkdownView> markdown, std::shared_ptr<const Localizer> text)
        : m_theme(theme)
        , m_markdown(std::move(markdown))
        , m_text(std::move(text)) {}

    void start(const domain::Quiz& quiz, const domain::TourProgress& progress);
    void restart();
    void next(const domain::Quiz& quiz);
    [[nodiscard]] bool previous();
    void move_cursor(int delta, const domain::Quiz& quiz);

    [[nodiscard]] std::optional<std::size_t> question(const domain::Quiz& quiz) const noexcept;

    [[nodiscard]] std::size_t cursor() const noexcept {
        return m_cursor;
    }

    [[nodiscard]] ftxui::Element render(const std::optional<domain::Quiz>& quiz, const domain::TourProgress& progress,
                                        std::string_view agent) const;

private:
    [[nodiscard]] ftxui::Element missing(std::string_view agent) const;
    [[nodiscard]] ftxui::Element asking(const domain::Quiz& quiz, const domain::TourProgress& progress) const;
    [[nodiscard]] ftxui::Element option(const domain::QuizQuestion& question, std::size_t index,
                                        std::optional<std::size_t> chosen) const;
    [[nodiscard]] ftxui::Element summary(const domain::Quiz& quiz, const domain::TourProgress& progress) const;

    Theme m_theme;
    std::shared_ptr<const MarkdownView> m_markdown;
    std::shared_ptr<const Localizer> m_text;
    std::size_t m_index = 0;
    std::size_t m_cursor = 0;
};

}
