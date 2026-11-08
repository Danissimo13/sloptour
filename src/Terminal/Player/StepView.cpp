#include "Terminal/Player/StepView.hpp"

#include "Terminal/Widgets/Chrome.hpp"
#include "Terminal/Widgets/Glyphs.hpp"

#include <format>

namespace sloptour::terminal {

ftxui::Element StepView::render(const domain::Tour& tour, const domain::TourProgress& progress,
                                domain::StepPosition position, std::size_t location,
                                const SourceLookup& sources) const {
    const auto& step = tour.step_at(position);
    ftxui::Elements blocks{heading(tour, position), ftxui::text(""), m_markdown->render(step.body())};
    if (!step.locations().empty()) {
        const auto index = std::min(location, step.locations().size() - 1);
        const auto& shown = step.locations().at(index);
        blocks.push_back(ftxui::text(""));
        blocks.push_back(m_code.render(shown, sources(shown.path()), index, step.locations().size()));
    }
    if (!step.commands().empty()) {
        blocks.push_back(ftxui::text(""));
        blocks.push_back(commands(step));
    }
    if (const auto& check = step.checkpoint()) {
        blocks.push_back(ftxui::text(""));
        blocks.push_back(checkpoint(*check, progress.is_revealed(step.id())));
    }
    blocks.push_back(ftxui::text(""));
    return ftxui::vbox(std::move(blocks));
}

ftxui::Element StepView::heading(const domain::Tour& tour, domain::StepPosition position) const {
    const auto& chapter = tour.chapters().at(position.chapter);
    return ftxui::vbox({
        ftxui::hbox({
            ftxui::text(m_text->format("step.chapter", {std::to_string(position.chapter + 1)})) |
                ftxui::color(m_theme.accent) | ftxui::bold,
            ftxui::text(std::string(glyphs::SEPARATOR) + chapter.title()) | ftxui::color(m_theme.muted),
            ftxui::filler(),
            ftxui::text(m_text->format("step.position",
                                       {std::to_string(position.step + 1), std::to_string(chapter.steps().size())})) |
                ftxui::color(m_theme.muted),
        }),
        ftxui::text(tour.step_at(position).title()) | ftxui::bold | ftxui::color(m_theme.heading),
    });
}

ftxui::Element StepView::commands(const domain::Step& step) const {
    ftxui::Elements rows{section_title(m_text->text("step.try_it"), m_theme)};
    for (const auto& command : step.commands()) {
        rows.push_back(ftxui::hbox({ftxui::text("  " + std::string(glyphs::RUN) + " ") | ftxui::color(m_theme.success),
                                    ftxui::text(command) | ftxui::color(m_theme.inline_code)}));
    }
    return ftxui::vbox(std::move(rows));
}

ftxui::Element StepView::checkpoint(const domain::Checkpoint& checkpoint, bool revealed) const {
    ftxui::Elements rows{
        section_title(m_text->text("step.check_yourself"), m_theme),
        ftxui::hbox(
            {ftxui::text("  " + std::string(glyphs::QUESTION) + " ") | ftxui::color(m_theme.warning) | ftxui::bold,
             m_markdown->inline_text(checkpoint.question) | ftxui::flex}),
    };
    if (revealed) {
        rows.push_back(ftxui::hbox({ftxui::text("    "), m_markdown->inline_text(checkpoint.answer) | ftxui::flex}) |
                       ftxui::color(m_theme.success));
    } else {
        rows.push_back(ftxui::text("    " + m_text->text("step.reveal_hint")) | ftxui::color(m_theme.muted) |
                       ftxui::italic);
    }
    return ftxui::vbox(std::move(rows));
}

}
