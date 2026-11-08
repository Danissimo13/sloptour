#pragma once

#include "Application/Library/TourSummary.hpp"
#include "Application/Ports/RepositoryOverview.hpp"
#include "Application/Preferences/StorageMode.hpp"
#include "Terminal/Shell/Destination.hpp"
#include "Terminal/Shell/TerminalContext.hpp"

#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>

#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace sloptour::terminal {

class HomeScreen {
public:
    HomeScreen(std::shared_ptr<const TerminalContext> context, std::optional<std::string> notice);

    [[nodiscard]] Destination run();

private:
    [[nodiscard]] ftxui::Element render() const;
    [[nodiscard]] ftxui::Element render_settings() const;
    [[nodiscard]] ftxui::Element render_choice(std::size_t row, std::string_view label,
                                               const std::vector<std::string>& options, std::size_t current,
                                               ftxui::Element note) const;
    [[nodiscard]] ftxui::Element render_tours() const;
    [[nodiscard]] ftxui::Element render_tour(std::size_t index) const;
    [[nodiscard]] ftxui::Element render_action(std::size_t index) const;
    [[nodiscard]] ftxui::Element render_welcome() const;
    [[nodiscard]] ftxui::Element repository_line() const;
    [[nodiscard]] std::string details_of(const application::TourSummary& tour) const;
    [[nodiscard]] bool handle(const ftxui::Event& event);
    [[nodiscard]] bool handle_setting(const ftxui::Event& event);
    [[nodiscard]] std::size_t row_count() const noexcept;
    [[nodiscard]] static std::size_t first_tour_row() noexcept;
    [[nodiscard]] std::size_t first_action_row() const noexcept;
    [[nodiscard]] std::size_t language_index() const noexcept;
    [[nodiscard]] std::size_t storage_index() const noexcept;
    void cycle_language(int direction);
    void cycle_storage(int direction);
    void reload();
    void choose();
    void leave(Destination destination);

    static constexpr std::size_t SETTINGS_ROWS = 2;

    std::shared_ptr<const TerminalContext> m_context;
    std::vector<application::TourSummary> m_tours;
    std::optional<application::RepositoryOverview> m_overview;
    std::optional<std::string> m_problem;
    std::size_t m_selected = SETTINGS_ROWS;
    Destination m_next = GoExit{};
};

}
