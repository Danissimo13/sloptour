#include "Terminal/Home/HomeScreen.hpp"

#include "Terminal/Widgets/Chrome.hpp"
#include "Terminal/Widgets/Glyphs.hpp"
#include "Terminal/Widgets/TourKindStyle.hpp"

#include <ftxui/component/component.hpp>

#include <array>
#include <format>

namespace sloptour::terminal {

namespace {

constexpr std::size_t LANGUAGE_ROW = 0;
constexpr std::size_t STORAGE_ROW = 1;
constexpr std::size_t TITLE_WIDTH = 52;
constexpr int KIND_WIDTH = 12;
constexpr int PROGRESS_WIDTH = 16;
constexpr int SETTING_LABEL_WIDTH = 18;

constexpr std::array STORAGE_MODES{
    std::pair{application::StorageMode::Repository, std::string_view("storage.repository")},
    std::pair{application::StorageMode::Global, std::string_view("storage.global")},
    std::pair{application::StorageMode::Memory, std::string_view("storage.memory")},
};

constexpr std::array HINTS{
    KeyHint{.key = "↑↓", .label = "hint.select"},  KeyHint{.key = "←→", .label = "hint.change"},
    KeyHint{.key = "enter", .label = "hint.open"}, KeyHint{.key = "n r f", .label = "hint.new_tour"},
    KeyHint{.key = "l", .label = "hint.language"}, KeyHint{.key = "s", .label = "hint.storage"},
    KeyHint{.key = "q", .label = "hint.quit"},
};

[[nodiscard]] std::optional<domain::TourKind> kind_for_shortcut(const ftxui::Event& event) {
    for (const auto& style : tour_kind_styles()) {
        if (event == ftxui::Event::Character(style.shortcut)) {
            return style.kind;
        }
    }
    return std::nullopt;
}

[[nodiscard]] std::size_t cycled(std::size_t current, std::size_t count, int direction) {
    return (current + (direction > 0 ? 1U : count - 1)) % count;
}

}

HomeScreen::HomeScreen(std::shared_ptr<const TerminalContext> context, std::optional<std::string> notice)
    : m_context(std::move(context))
    , m_problem(std::move(notice)) {
    reload();
    if (auto overview = m_context->services.workspace->overview()) {
        m_overview = std::move(*overview);
    }
}

Destination HomeScreen::run() {
    auto component = ftxui::Renderer([this] { return render(); });
    component |= ftxui::CatchEvent([this](const ftxui::Event& event) { return handle(event); });
    m_context->controls.loop(component);
    return m_next;
}

void HomeScreen::reload() {
    auto tours = m_context->services.library->summaries();
    if (tours) {
        m_tours = std::move(*tours);
    } else {
        m_tours.clear();
        m_problem = tours.error().message();
    }
    m_selected = std::min(m_selected, row_count() - 1);
}

ftxui::Element HomeScreen::render() const {
    const auto& theme = m_context->theme;
    const auto& text = *m_context->localizer;
    auto body = m_tours.empty() ? render_welcome() : render_tours();
    ftxui::Elements rows{header_bar(text.text("home.tagline"), repository_line(), theme), render_settings(),
                         std::move(body) | ftxui::flex};
    if (m_problem) {
        rows.push_back(ftxui::text(" " + *m_problem) | ftxui::color(theme.danger));
    }
    rows.push_back(ftxui::separatorLight() | ftxui::color(theme.accent_soft));
    rows.push_back(key_hints(HINTS, theme, text));
    return ftxui::vbox(std::move(rows)) | ftxui::bgcolor(theme.background);
}

ftxui::Element HomeScreen::render_settings() const {
    const auto& theme = m_context->theme;
    const auto& text = *m_context->localizer;
    std::vector<std::string> languages;
    languages.reserve(text.packs().size());
    for (const auto& pack : text.packs()) {
        languages.push_back(pack.label);
    }
    std::vector<std::string> modes;
    modes.reserve(STORAGE_MODES.size());
    for (const auto& [mode, key] : STORAGE_MODES) {
        modes.push_back(text.text(key));
    }
    const auto mode = STORAGE_MODES.at(storage_index()).first;
    const auto location = m_context->services.preferences->storage_location(mode);
    auto note = ftxui::text(location.empty() ? text.text("storage.memory_note") : location) | ftxui::color(theme.muted);
    return ftxui::vbox({
        section_title("  " + text.text("home.settings"), theme),
        render_choice(LANGUAGE_ROW, text.text("home.language"), languages, language_index(),
                      ftxui::text(text.text("home.language_note")) | ftxui::color(theme.muted)),
        render_choice(STORAGE_ROW, text.text("home.storage"), modes, storage_index(), std::move(note)),
        ftxui::text(""),
    });
}

ftxui::Element HomeScreen::render_choice(std::size_t row, std::string_view label,
                                         const std::vector<std::string>& options, std::size_t current,
                                         ftxui::Element note) const {
    const auto& theme = m_context->theme;
    const bool selected = row == m_selected;
    ftxui::Elements parts{
        selection_marker(selected, theme),
        ftxui::text(std::string(label)) | ftxui::color(selected ? theme.accent : theme.text) |
            ftxui::size(ftxui::WIDTH, ftxui::EQUAL, SETTING_LABEL_WIDTH),
    };
    for (std::size_t index = 0; index < options.size(); ++index) {
        auto option = ftxui::text(" " + options.at(index) + " ");
        option = index == current ? std::move(option) | ftxui::bold | ftxui::inverted | ftxui::color(theme.accent)
                                  : std::move(option) | ftxui::color(theme.muted);
        parts.push_back(std::move(option));
        parts.push_back(ftxui::text(" "));
    }
    auto line = ftxui::vbox({
        ftxui::hbox(std::move(parts)),
        ftxui::hbox({ftxui::text("   "), ftxui::text("") | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, SETTING_LABEL_WIDTH),
                     std::move(note) | ftxui::flex}),
    });
    return selected ? std::move(line) | ftxui::focus : line;
}

ftxui::Element HomeScreen::render_tours() const {
    const auto& theme = m_context->theme;
    const auto& text = *m_context->localizer;
    ftxui::Elements rows{section_title("  " + text.text("home.your_tours"), theme)};
    for (std::size_t index = 0; index < m_tours.size(); ++index) {
        rows.push_back(render_tour(index));
    }
    rows.push_back(ftxui::text(""));
    rows.push_back(section_title("  " + text.text("home.start_new"), theme));
    for (std::size_t index = 0; index < tour_kind_styles().size(); ++index) {
        rows.push_back(render_action(index));
    }
    return ftxui::vbox(std::move(rows)) | ftxui::yframe;
}

ftxui::Element HomeScreen::render_tour(std::size_t index) const {
    const auto& theme = m_context->theme;
    const auto& tour = m_tours.at(index);
    const bool selected = first_tour_row() + index == m_selected;
    const bool finished = tour.visited == tour.steps;
    auto row = ftxui::hbox({
        selection_marker(selected, theme),
        kind_badge(tour.metadata.kind, theme, *m_context->localizer) |
            ftxui::size(ftxui::WIDTH, ftxui::EQUAL, KIND_WIDTH),
        ftxui::text(" " + shortened(tour.metadata.title, TITLE_WIDTH)) | ftxui::bold |
            ftxui::color(selected ? theme.accent : theme.text),
        ftxui::filler(),
        ftxui::gauge(tour.steps == 0 ? 0.0F : static_cast<float>(tour.visited) / static_cast<float>(tour.steps)) |
            ftxui::color(finished ? theme.success : theme.accent) |
            ftxui::size(ftxui::WIDTH, ftxui::EQUAL, PROGRESS_WIDTH),
        ftxui::text(std::format(" {:>3}/{:<3} ", tour.visited, tour.steps)) | ftxui::color(theme.muted),
        ftxui::text(tour.metadata.created_at + " ") | ftxui::color(theme.muted),
    });
    auto details = ftxui::text("      " + details_of(tour)) | ftxui::color(theme.muted);
    auto entry = ftxui::vbox({std::move(row), std::move(details)});
    return selected ? entry | ftxui::focus : entry;
}

ftxui::Element HomeScreen::render_action(std::size_t index) const {
    const auto& theme = m_context->theme;
    const bool selected = first_action_row() + index == m_selected;
    const auto label = m_context->localizer->text(style_key(style_at(index), "action"));
    auto row = ftxui::hbox({
        selection_marker(selected, theme),
        ftxui::text(std::string(glyphs::ADD) + " ") | ftxui::color(theme.success) | ftxui::bold,
        ftxui::text(label) | ftxui::color(selected ? theme.accent : theme.text) |
            (selected ? ftxui::bold : ftxui::nothing),
    });
    return selected ? row | ftxui::focus : row;
}

ftxui::Element HomeScreen::render_welcome() const {
    const auto& theme = m_context->theme;
    const auto& text = *m_context->localizer;
    ftxui::Elements rows{
        ftxui::text(text.text("home.welcome")) | ftxui::bold | ftxui::color(theme.heading) | ftxui::hcenter,
        ftxui::text(text.text("home.welcome_note")) | ftxui::color(theme.muted) | ftxui::hcenter,
        ftxui::text(""),
    };
    for (std::size_t index = 0; index < tour_kind_styles().size(); ++index) {
        rows.push_back(render_action(index));
    }
    return ftxui::vbox(std::move(rows)) | ftxui::borderStyled(ftxui::ROUNDED, theme.accent_soft) | ftxui::center;
}

ftxui::Element HomeScreen::repository_line() const {
    const auto& theme = m_context->theme;
    if (!m_overview) {
        return ftxui::text("");
    }
    auto line = m_overview->name;
    if (m_overview->versioned) {
        line += std::format("{}{}", glyphs::SEPARATOR, m_overview->branch);
    }
    return ftxui::text(line) | ftxui::color(theme.muted);
}

std::string HomeScreen::details_of(const application::TourSummary& tour) const {
    const auto& text = *m_context->localizer;
    const auto& metadata = tour.metadata;
    auto details = text.count(tour.chapters, "chapter");
    if (tour.has_quiz) {
        details += " + " + text.text("home.quiz");
    }
    details += std::string(glyphs::SEPARATOR) + metadata.scope + std::string(glyphs::SEPARATOR) +
               text.format("home.by_agent", {metadata.agent});
    if (!metadata.model.empty()) {
        details += " (" + metadata.model + ")";
    }
    if (!metadata.depth.empty()) {
        details += std::string(glyphs::SEPARATOR) + text.format("home.levels", {text.text("level." + metadata.depth),
                                                                                text.text("level." + metadata.detail)});
    }
    if (!metadata.language.empty()) {
        details += std::string(glyphs::SEPARATOR) + metadata.language;
    }
    return details;
}

bool HomeScreen::handle(const ftxui::Event& event) {
    if (event == ftxui::Event::ArrowDown || event == ftxui::Event::Character('j')) {
        m_selected = (m_selected + 1) % row_count();
    } else if (event == ftxui::Event::ArrowUp || event == ftxui::Event::Character('k')) {
        m_selected = (m_selected + row_count() - 1) % row_count();
    } else if (handle_setting(event)) {
        return true;
    } else if (event == ftxui::Event::Return) {
        choose();
    } else if (const auto kind = kind_for_shortcut(event)) {
        leave(GoWizard{.kind = *kind});
    } else if (event == ftxui::Event::Character('q') || event == ftxui::Event::Escape) {
        leave(GoExit{});
    } else {
        return false;
    }
    return true;
}

bool HomeScreen::handle_setting(const ftxui::Event& event) {
    const bool left = event == ftxui::Event::ArrowLeft;
    const bool right = event == ftxui::Event::ArrowRight || event == ftxui::Event::Return;
    const int direction = left ? -1 : 1;
    if (event == ftxui::Event::Character('l') || (m_selected == LANGUAGE_ROW && (left || right))) {
        cycle_language(direction);
        return true;
    }
    if (event == ftxui::Event::Character('s') || (m_selected == STORAGE_ROW && (left || right))) {
        cycle_storage(direction);
        return true;
    }
    return false;
}

void HomeScreen::cycle_language(int direction) {
    auto& text = *m_context->localizer;
    const auto& packs = text.packs();
    const auto& next = packs.at(cycled(language_index(), packs.size(), direction));
    std::ignore = text.use(next.code);
    if (auto saved = m_context->services.preferences->choose_language(next.code); !saved) {
        m_problem = saved.error().message();
    }
}

void HomeScreen::cycle_storage(int direction) {
    const auto mode = STORAGE_MODES.at(cycled(storage_index(), STORAGE_MODES.size(), direction)).first;
    if (auto saved = m_context->services.preferences->choose_storage(mode); !saved) {
        m_problem = saved.error().message();
    }
    reload();
}

std::size_t HomeScreen::language_index() const noexcept {
    const auto& text = *m_context->localizer;
    const auto& packs = text.packs();
    for (std::size_t index = 0; index < packs.size(); ++index) {
        if (packs.at(index).code == text.current().code) {
            return index;
        }
    }
    return 0;
}

std::size_t HomeScreen::storage_index() const noexcept {
    const auto mode = m_context->services.preferences->current().storage;
    for (std::size_t index = 0; index < STORAGE_MODES.size(); ++index) {
        if (STORAGE_MODES.at(index).first == mode) {
            return index;
        }
    }
    return 0;
}

std::size_t HomeScreen::row_count() const noexcept {
    return first_action_row() + tour_kind_styles().size();
}

std::size_t HomeScreen::first_tour_row() noexcept {
    return SETTINGS_ROWS;
}

std::size_t HomeScreen::first_action_row() const noexcept {
    return SETTINGS_ROWS + m_tours.size();
}

void HomeScreen::choose() {
    if (m_selected >= first_tour_row() && m_selected < first_action_row()) {
        leave(GoPlay{.tour = m_tours.at(m_selected - first_tour_row()).id});
        return;
    }
    if (m_selected >= first_action_row()) {
        leave(GoWizard{.kind = style_at(m_selected - first_action_row()).kind});
    }
}

void HomeScreen::leave(Destination destination) {
    m_next = std::move(destination);
    m_context->controls.exit();
}

}
