#include "Terminal/Player/OutlineView.hpp"

#include "Terminal/Widgets/Chrome.hpp"
#include "Terminal/Widgets/Glyphs.hpp"

#include <algorithm>
#include <format>

namespace sloptour::terminal {

namespace {

constexpr std::size_t CHAPTER_INDENT = 5;
constexpr std::size_t STEP_INDENT = 8;

}

ftxui::Element OutlineView::render(const domain::Tour& tour, const domain::TourProgress& progress,
                                   std::optional<domain::StepPosition> current) const {
    ftxui::Elements rows;
    for (std::size_t index = 0; index < tour.chapters().size(); ++index) {
        rows.push_back(chapter(tour, progress, index, current));
    }
    rows.push_back(quiz(tour, progress, !current.has_value()));
    return ftxui::vbox(std::move(rows)) | ftxui::vscroll_indicator | ftxui::yframe |
           ftxui::size(ftxui::WIDTH, ftxui::EQUAL, static_cast<int>(m_width));
}

ftxui::Element OutlineView::quiz(const domain::Tour& tour, const domain::TourProgress& progress, bool active) const {
    auto label = std::string(glyphs::STAR) + " " + m_text->text("quiz.title");
    if (const auto& questions = tour.quiz()) {
        const auto& answers = progress.snapshot().quiz_answers;
        if (!answers.empty()) {
            label += std::format("  {}/{}", questions->score(answers), questions->questions().size());
        }
    }
    auto row =
        ftxui::hbox({ftxui::text(active ? std::string(glyphs::POINTER) : " ") | ftxui::color(m_theme.accent),
                     ftxui::text(label) | ftxui::bold | ftxui::color(active ? m_theme.accent : m_theme.warning)});
    return active ? row | ftxui::focus : row;
}

ftxui::Element OutlineView::chapter(const domain::Tour& tour, const domain::TourProgress& progress, std::size_t index,
                                    std::optional<domain::StepPosition> current) const {
    const auto& chapter = tour.chapters().at(index);
    const auto& steps = chapter.steps();
    const auto visited = static_cast<std::size_t>(
        std::ranges::count_if(steps, [&progress](const domain::Step& step) { return progress.is_visited(step.id()); }));
    const bool active = current && index == current->chapter;
    const bool complete = visited == steps.size();
    ftxui::Elements rows{ftxui::hbox({
        ftxui::text(std::format(" {:>2} ", index + 1)) | ftxui::color(active ? m_theme.accent : m_theme.muted) |
            ftxui::bold,
        ftxui::text(shortened(chapter.title(), m_width - CHAPTER_INDENT)) |
            ftxui::color(title_color(complete, active)) | ftxui::bold,
    })};
    for (std::size_t position = 0; position < steps.size(); ++position) {
        const bool here = active && position == current->step;
        rows.push_back(step(steps.at(position), progress.is_visited(steps.at(position).id()), here));
    }
    rows.push_back(ftxui::text(""));
    return ftxui::vbox(std::move(rows));
}

ftxui::Color OutlineView::title_color(bool complete, bool active) const {
    if (complete) {
        return m_theme.success;
    }
    return active ? m_theme.heading : m_theme.text;
}

ftxui::Element OutlineView::step(const domain::Step& step, bool visited, bool active) const {
    ftxui::Element icon = ftxui::text(std::string(glyphs::PENDING)) | ftxui::color(m_theme.muted);
    if (active) {
        icon = ftxui::text(std::string(glyphs::CURRENT)) | ftxui::color(m_theme.accent);
    } else if (visited) {
        icon = ftxui::text(std::string(glyphs::DONE)) | ftxui::color(m_theme.success);
    }
    auto row = ftxui::hbox({
        ftxui::text(active ? "  " + std::string(glyphs::POINTER) + " " : "    ") | ftxui::color(m_theme.accent),
        std::move(icon),
        ftxui::text(" " + shortened(step.title(), m_width - STEP_INDENT)) |
            ftxui::color(active ? m_theme.accent : m_theme.muted) | (active ? ftxui::bold : ftxui::nothing),
    });
    return active ? row | ftxui::focus : row;
}

}
