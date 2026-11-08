#include "Terminal/Player/QuizPanel.hpp"

#include "Terminal/Widgets/Chrome.hpp"
#include "Terminal/Widgets/Glyphs.hpp"

#include <format>

namespace sloptour::terminal {

namespace {

constexpr std::size_t PERCENT = 100;
constexpr std::size_t GREAT_PERCENT = 80;
constexpr std::size_t SOLID_PERCENT = 50;
constexpr std::string_view GREAT = "quiz.verdict_great";
constexpr std::string_view SOLID = "quiz.verdict_solid";
constexpr std::string_view AGAIN = "quiz.verdict_again";

[[nodiscard]] std::string_view verdict(std::size_t percent) {
    if (percent >= GREAT_PERCENT) {
        return GREAT;
    }
    return percent >= SOLID_PERCENT ? SOLID : AGAIN;
}

}

void QuizPanel::start(const domain::Quiz& quiz, const domain::TourProgress& progress) {
    m_index = 0;
    while (m_index < quiz.questions().size() && progress.quiz_answer(m_index)) {
        ++m_index;
    }
    m_cursor = 0;
}

void QuizPanel::restart() {
    m_index = 0;
    m_cursor = 0;
}

void QuizPanel::next(const domain::Quiz& quiz) {
    if (m_index < quiz.questions().size()) {
        ++m_index;
        m_cursor = 0;
    }
}

bool QuizPanel::previous() {
    if (m_index == 0) {
        return false;
    }
    --m_index;
    m_cursor = 0;
    return true;
}

void QuizPanel::move_cursor(int delta, const domain::Quiz& quiz) {
    const auto current = question(quiz);
    if (!current) {
        return;
    }
    const auto count = quiz.questions().at(*current).options().size();
    m_cursor = (m_cursor + (delta > 0 ? 1U : count - 1)) % count;
}

std::optional<std::size_t> QuizPanel::question(const domain::Quiz& quiz) const noexcept {
    if (m_index >= quiz.questions().size()) {
        return std::nullopt;
    }
    return m_index;
}

ftxui::Element QuizPanel::render(const std::optional<domain::Quiz>& quiz, const domain::TourProgress& progress,
                                 std::string_view agent) const {
    if (!quiz) {
        return missing(agent);
    }
    return question(*quiz) ? asking(*quiz, progress) : summary(*quiz, progress);
}

ftxui::Element QuizPanel::missing(std::string_view agent) const {
    return ftxui::vbox({
        ftxui::text(std::string(glyphs::STAR) + " " + m_text->text("quiz.title")) | ftxui::bold |
            ftxui::color(m_theme.heading),
        ftxui::text(""),
        ftxui::paragraph(m_text->format("quiz.missing", {agent})) | ftxui::color(m_theme.text),
    });
}

ftxui::Element QuizPanel::asking(const domain::Quiz& quiz, const domain::TourProgress& progress) const {
    const auto& current = quiz.questions().at(m_index);
    const auto chosen = progress.quiz_answer(m_index);
    ftxui::Elements rows{
        ftxui::hbox({ftxui::text(std::string(glyphs::STAR) + " " + m_text->text("quiz.title")) | ftxui::bold |
                         ftxui::color(m_theme.accent),
                     ftxui::filler(),
                     ftxui::text(m_text->format(
                         "quiz.position", {std::to_string(m_index + 1), std::to_string(quiz.questions().size())})) |
                         ftxui::color(m_theme.muted)}),
        ftxui::text(""),
        m_markdown->inline_text(current.question()) | ftxui::bold,
        ftxui::text(""),
    };
    for (std::size_t index = 0; index < current.options().size(); ++index) {
        rows.push_back(option(current, index, chosen));
    }
    rows.push_back(ftxui::text(""));
    if (chosen) {
        const bool right = current.is_correct(*chosen);
        rows.push_back(ftxui::text(right ? std::string(glyphs::DONE) + " " + m_text->text("quiz.correct")
                                         : std::string(glyphs::FAILED) + " " + m_text->text("quiz.wrong")) |
                       ftxui::bold | ftxui::color(right ? m_theme.success : m_theme.danger));
        rows.push_back(m_markdown->inline_text(current.explanation()));
        rows.push_back(ftxui::text(""));
        rows.push_back(ftxui::text(m_text->text("quiz.next_hint")) | ftxui::color(m_theme.muted));
    } else {
        rows.push_back(ftxui::text(m_text->text("quiz.answer_hint")) | ftxui::color(m_theme.muted));
    }
    return ftxui::vbox(std::move(rows));
}

ftxui::Element QuizPanel::option(const domain::QuizQuestion& question, std::size_t index,
                                 std::optional<std::size_t> chosen) const {
    auto row = ftxui::hbox({
        ftxui::text(std::format("  [{}] ", index + 1)) | ftxui::color(m_theme.accent),
        m_markdown->inline_text(question.options().at(index)) | ftxui::flex,
    });
    if (!chosen) {
        if (index != m_cursor) {
            return ftxui::hbox({ftxui::text("  "), row});
        }
        return ftxui::hbox({ftxui::text(std::string(glyphs::POINTER) + " ") | ftxui::color(m_theme.accent), row}) |
               ftxui::color(m_theme.accent) | ftxui::bold;
    }
    if (question.is_correct(index)) {
        return ftxui::hbox({ftxui::text("  "), row | ftxui::flex, ftxui::text(" " + std::string(glyphs::DONE) + " ")}) |
               ftxui::color(m_theme.success);
    }
    if (index == *chosen) {
        return ftxui::hbox(
                   {ftxui::text("  "), row | ftxui::flex, ftxui::text(" " + std::string(glyphs::FAILED) + " ")}) |
               ftxui::color(m_theme.danger);
    }
    return ftxui::hbox({ftxui::text("  "), row}) | ftxui::dim;
}

ftxui::Element QuizPanel::summary(const domain::Quiz& quiz, const domain::TourProgress& progress) const {
    const auto total = quiz.questions().size();
    const auto score = quiz.score(progress.snapshot().quiz_answers);
    const auto percent = score * PERCENT / total;
    ftxui::Elements rows{
        ftxui::text(std::string(glyphs::STAR) + " " + m_text->text("quiz.results")) | ftxui::bold |
            ftxui::color(m_theme.heading),
        ftxui::text(""),
        ftxui::text(
            m_text->format("quiz.score", {std::to_string(score), std::to_string(total), std::to_string(percent)})) |
            ftxui::bold | ftxui::color(percent >= GREAT_PERCENT ? m_theme.success : m_theme.warning),
        ftxui::text(m_text->text(verdict(percent))) | ftxui::color(m_theme.text),
        ftxui::text(""),
    };
    for (std::size_t index = 0; index < total; ++index) {
        const auto answer = progress.quiz_answer(index);
        const bool right = answer && quiz.questions().at(index).is_correct(*answer);
        rows.push_back(ftxui::hbox({
            ftxui::text(right ? " " + std::string(glyphs::DONE) + " " : " " + std::string(glyphs::FAILED) + " ") |
                ftxui::color(right ? m_theme.success : m_theme.danger),
            m_markdown->inline_text(quiz.questions().at(index).question()) | ftxui::flex,
        }));
    }
    rows.push_back(ftxui::text(""));
    rows.push_back(ftxui::text(m_text->text("quiz.summary_hint")) | ftxui::color(m_theme.muted));
    return ftxui::vbox(std::move(rows));
}

}
