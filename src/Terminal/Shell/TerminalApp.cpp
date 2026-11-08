#include "Terminal/Shell/TerminalApp.hpp"

#include "Terminal/Shell/Navigator.hpp"
#include "Terminal/Theme/ThemeFactory.hpp"

#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <tuple>

namespace sloptour::terminal {

void TerminalApp::run(Destination first) const {
    auto screen = ftxui::ScreenInteractive::Fullscreen();
    const ScreenControls controls{
        .loop = [&screen](ftxui::Component component) { screen.Loop(std::move(component)); },
        .exit = [&screen] { screen.Exit(); },
        .post = [&screen](std::function<void()> task) { screen.Post(std::move(task)); },
        .redraw = [&screen] { screen.PostEvent(ftxui::Event::Custom); },
        .suspended = [&screen](std::function<void()> task) { screen.WithRestoredIO(std::move(task))(); },
    };
    const auto theme = theme_from(m_settings.palette);
    auto highlighter = std::make_shared<const SyntaxHighlighter>(m_settings.languages, theme);
    auto markdown = std::make_shared<const MarkdownView>(theme, highlighter);
    auto localizer = std::make_shared<Localizer>(m_settings.packs);
    std::ignore = localizer->use(m_services.preferences->current().language);
    const Navigator navigator(std::make_shared<const TerminalContext>(TerminalContext{.controls = controls,
                                                                                      .services = m_services,
                                                                                      .settings = m_settings,
                                                                                      .theme = theme,
                                                                                      .highlighter = highlighter,
                                                                                      .markdown = markdown,
                                                                                      .localizer = localizer}));
    auto destination = std::move(first);
    while (!std::holds_alternative<GoExit>(destination)) {
        destination = std::visit(navigator, destination);
    }
}

}
