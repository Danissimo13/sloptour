#include "Terminal/Wizard/WizardScreen.hpp"

#include "Core/Text/Text.hpp"
#include "Terminal/Widgets/Chrome.hpp"
#include "Terminal/Widgets/Glyphs.hpp"
#include "Terminal/Widgets/TourKindStyle.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>

#include <algorithm>
#include <array>
#include <format>

namespace sloptour::terminal {

namespace {

constexpr std::size_t DEFAULT_RECAP_COMMITS = 5;
constexpr int LABEL_WIDTH = 16;
constexpr std::size_t SUBJECT_WIDTH = 64;

constexpr std::array HINTS{
    KeyHint{.key = "tab/↑↓", .label = "hint.field"}, KeyHint{.key = "←→", .label = "hint.choose"},
    KeyHint{.key = "space", .label = "hint.toggle"}, KeyHint{.key = "enter", .label = "hint.press"},
    KeyHint{.key = "esc", .label = "hint.back"},
};

[[nodiscard]] std::string commit_label(const application::CommitSummary& commit) {
    return std::format("{}  {}  ({}, {})", commit.hash, shortened(commit.subject, SUBJECT_WIDTH), commit.author,
                       commit.date);
}

[[nodiscard]] int index_of(const std::vector<std::string>& labels, const std::string& wanted) {
    const auto found = std::ranges::find(labels, wanted);
    return found == labels.end() ? 0 : static_cast<int>(std::distance(labels.begin(), found));
}

[[nodiscard]] int preferred_agent(const std::vector<application::AgentDescriptor>& agents, std::string_view name) {
    const auto found = std::ranges::find(agents, name, &application::AgentDescriptor::name);
    return found == agents.end() ? 0 : static_cast<int>(std::distance(agents.begin(), found));
}

}

WizardScreen::WizardScreen(std::shared_ptr<const TerminalContext> context, domain::TourKind kind)
    : m_context(std::move(context))
    , m_kind(kind)
    , m_agents(m_context->services.workspace->agents()) {
    for (const auto& agent : m_agents) {
        m_agent_labels.push_back(agent.name + (agent.installed ? "" : "  " + t("wizard.not_installed")));
    }
    m_agent_index = preferred_agent(m_agents, m_context->services.workspace->default_agent());
    if (auto commits = m_context->services.workspace->recent_commits(m_context->settings.recent_commits)) {
        m_commits = std::move(*commits);
    }
    m_commit_labels.push_back(t("wizard.only_working_tree"));
    for (const auto& commit : m_commits) {
        m_commit_labels.push_back(commit_label(commit));
    }
    m_commit_index = static_cast<int>(std::min(m_commits.size(), DEFAULT_RECAP_COMMITS));
    const auto& workspace = *m_context->services.workspace;
    m_depth_names = workspace.depth_levels();
    m_detail_names = workspace.detail_levels();
    for (const auto& name : m_depth_names) {
        m_depth_labels.push_back(t("level." + name));
    }
    for (const auto& name : m_detail_names) {
        m_detail_labels.push_back(t("level." + name));
    }
    m_depth_index = index_of(m_depth_names, workspace.default_level());
    m_detail_index = index_of(m_detail_names, workspace.default_level());
    sync_models();
}

void WizardScreen::sync_models() {
    m_model_labels = {t("wizard.default_model")};
    if (!m_agents.empty()) {
        const auto& models = m_agents.at(static_cast<std::size_t>(m_agent_index)).models;
        m_model_labels.insert(m_model_labels.end(), models.begin(), models.end());
    }
    m_model_index = 0;
}

Destination WizardScreen::run() {
    auto component = build();
    m_context->controls.loop(component);
    return m_next;
}

void WizardScreen::build_controls() {
    auto agent_option = ftxui::MenuOption::Toggle();
    agent_option.on_change = [this] { sync_models(); };
    m_agent_menu = ftxui::Menu(&m_agent_labels, &m_agent_index, agent_option);
    m_model_menu = ftxui::Toggle(&m_model_labels, &m_model_index);
    m_depth_menu = ftxui::Toggle(&m_depth_labels, &m_depth_index);
    m_detail_menu = ftxui::Toggle(&m_detail_labels, &m_detail_index);
    m_commit_menu = ftxui::Renderer([this](bool focused) { return render_since(focused); }) |
                    ftxui::CatchEvent([this](const ftxui::Event& event) { return step_since(event); });
    m_working_tree = ftxui::Checkbox(t("wizard.include_working_tree"), &m_include_working_tree);
    auto single_line = ftxui::InputOption::Default();
    single_line.multiline = false;
    single_line.placeholder = t("wizard.focus_placeholder");
    m_hint_input = ftxui::Input(&m_hint, single_line);
    single_line.placeholder = t("wizard.functionality_placeholder");
    m_functionality_input = ftxui::Input(&m_functionality, single_line);
    single_line.placeholder = t("wizard.name_placeholder");
    m_name_input = ftxui::Input(&m_name, single_line);
    m_buttons = ftxui::Container::Horizontal({
        ftxui::Button(
            " " + t("wizard.start") + " ", [this] { submit(); }, ftxui::ButtonOption::Animated()),
        ftxui::Button(
            " " + t("wizard.cancel") + " ", [this] { leave(GoHome{}); }, ftxui::ButtonOption::Animated()),
    });
}

ftxui::Component WizardScreen::build() {
    build_controls();
    ftxui::Components fields;
    if (m_kind == domain::TourKind::Functional) {
        fields.push_back(m_functionality_input);
    }
    fields.push_back(m_agent_menu);
    fields.push_back(m_model_menu);
    if (m_kind == domain::TourKind::Recap) {
        fields.push_back(m_commit_menu);
        fields.push_back(m_working_tree);
    }
    fields.push_back(m_depth_menu);
    fields.push_back(m_detail_menu);
    if (m_kind != domain::TourKind::Functional) {
        fields.push_back(m_hint_input);
    }
    fields.push_back(m_name_input);
    fields.push_back(m_buttons);
    auto container = ftxui::Container::Vertical(std::move(fields));
    auto component = ftxui::Renderer(container, [this] { return render(); });
    return component | ftxui::CatchEvent([this](const ftxui::Event& event) {
               if (event == ftxui::Event::Escape) {
                   leave(GoHome{});
                   return true;
               }
               return false;
           });
}

ftxui::Element WizardScreen::render() const {
    const auto& theme = m_context->theme;
    const auto& style = style_of(m_kind);
    ftxui::Elements form{
        ftxui::text(t(style_key(style, "title"))) | ftxui::bold | ftxui::color(theme.heading),
        ftxui::paragraph(t(style_key(style, "description"))) | ftxui::color(theme.muted),
        ftxui::text(""),
    };
    for (auto& element : kind_fields()) {
        form.push_back(std::move(element));
    }
    form.push_back(field(t("wizard.depth"), t("wizard.depth_help"), m_depth_menu->Render()));
    form.push_back(field(t("wizard.detail"), t("wizard.detail_help"), m_detail_menu->Render()));
    if (m_kind != domain::TourKind::Functional) {
        form.push_back(field(t("wizard.focus"), t("wizard.focus_help"), m_hint_input->Render()));
    }
    form.push_back(field(t("wizard.name"), "", m_name_input->Render()));
    form.push_back(ftxui::text(""));
    form.push_back(m_buttons->Render() | ftxui::hcenter);
    if (m_problem) {
        form.push_back(ftxui::text(*m_problem) | ftxui::color(theme.danger) | ftxui::hcenter);
    }
    return ftxui::vbox({
               header_bar(t(style_key(style, "header")), ftxui::text(""), theme),
               ftxui::vbox(std::move(form)) | ftxui::borderStyled(ftxui::ROUNDED, theme.accent_soft) | ftxui::center |
                   ftxui::flex,
               ftxui::separatorLight() | ftxui::color(theme.accent_soft),
               key_hints(HINTS, theme, *m_context->localizer),
           }) |
           ftxui::bgcolor(theme.background);
}

ftxui::Elements WizardScreen::kind_fields() const {
    ftxui::Elements fields;
    if (m_kind == domain::TourKind::Functional) {
        fields.push_back(
            field(t("wizard.functionality"), t("wizard.functionality_help"), m_functionality_input->Render()));
    }
    fields.push_back(field(t("wizard.agent"), t("wizard.agent_help"), m_agent_menu->Render()));
    fields.push_back(field(t("wizard.model"), t("wizard.model_help"), m_model_menu->Render()));
    if (m_kind == domain::TourKind::Recap) {
        fields.push_back(field(t("wizard.since"), t("wizard.since_help"), m_commit_menu->Render()));
        fields.push_back(field(t("wizard.working_tree"), "", m_working_tree->Render()));
    }
    return fields;
}

ftxui::Element WizardScreen::render_since(bool focused) const {
    const auto& theme = m_context->theme;
    const auto index = static_cast<std::size_t>(m_commit_index);
    const auto last = m_commit_labels.size() - 1;
    const auto arrow = [&theme, focused](bool available, std::string_view glyph) {
        return ftxui::text(std::string(glyph)) | ftxui::color(available && focused ? theme.accent : theme.muted);
    };
    auto label = ftxui::text(m_commit_labels.at(index)) | ftxui::color(focused ? theme.accent : theme.text);
    const auto tree = m_include_working_tree ? " + " + t("wizard.uncommitted") : std::string();
    const auto covered =
        index == 0
            ? t("wizard.uncommitted_only")
            : m_context->localizer->format("wizard.explains", {m_context->localizer->count(index - 1, "commit"), tree});
    return ftxui::hbox({
        arrow(index > 0, "◀ "),
        focused ? std::move(label) | ftxui::bold : std::move(label),
        arrow(index < last, " ▶"),
        ftxui::filler(),
        ftxui::text(std::format("  {}  {}/{}", covered, index + 1, last + 1)) | ftxui::color(theme.muted),
    });
}

bool WizardScreen::step_since(const ftxui::Event& event) {
    const auto last = static_cast<int>(m_commit_labels.size()) - 1;
    if (event == ftxui::Event::ArrowLeft || event == ftxui::Event::Character('h')) {
        m_commit_index = std::max(0, m_commit_index - 1);
        return true;
    }
    if (event == ftxui::Event::ArrowRight || event == ftxui::Event::Character('l')) {
        m_commit_index = std::min(last, m_commit_index + 1);
        return true;
    }
    return false;
}

ftxui::Element WizardScreen::field(std::string_view label, std::string_view help, ftxui::Element control) const {
    const auto& theme = m_context->theme;
    ftxui::Elements column{std::move(control)};
    if (!help.empty()) {
        column.push_back(ftxui::text(std::string(help)) | ftxui::color(theme.muted) | ftxui::dim);
    }
    return ftxui::hbox({
               ftxui::text(std::string(label)) | ftxui::bold | ftxui::color(theme.accent) |
                   ftxui::size(ftxui::WIDTH, ftxui::EQUAL, LABEL_WIDTH),
               ftxui::vbox(std::move(column)) | ftxui::flex,
           }) |
           ftxui::borderEmpty;
}

application::TourFocus WizardScreen::focus() const {
    if (m_kind == domain::TourKind::Functional) {
        return application::FunctionalFocus{.functionality = m_functionality};
    }
    if (m_kind == domain::TourKind::Recap) {
        const auto index = static_cast<std::size_t>(m_commit_index);
        return application::RecapFocus{
            .scope = {.base_revision = index == 0 ? std::nullopt : std::optional(m_commits.at(index - 1).hash),
                      .include_working_tree = m_include_working_tree || index == 0}};
    }
    return application::OnboardingFocus{};
}

application::GenerationRequest WizardScreen::request() const {
    const auto& agent = m_agents.at(static_cast<std::size_t>(m_agent_index));
    const auto model = m_model_index == 0 ? std::string() : m_model_labels.at(static_cast<std::size_t>(m_model_index));
    return {.focus = focus(),
            .agent = {.name = agent.name, .model = model},
            .hint = m_hint,
            .name = m_name.empty() ? std::nullopt : std::optional(m_name),
            .depth = m_depth_names.at(static_cast<std::size_t>(m_depth_index)),
            .detail = m_detail_names.at(static_cast<std::size_t>(m_detail_index)),
            .language = m_context->localizer->reply_language()};
}

void WizardScreen::submit() {
    if (m_agents.empty()) {
        m_problem = t("wizard.no_agents");
        return;
    }
    if (m_kind == domain::TourKind::Functional && core::is_blank(m_functionality)) {
        m_problem = t("wizard.functionality_required");
        m_functionality_input->TakeFocus();
        return;
    }
    const auto& agent = m_agents.at(static_cast<std::size_t>(m_agent_index));
    if (!agent.installed) {
        m_problem = m_context->localizer->format("wizard.not_on_path", {agent.executable});
        return;
    }
    leave(GoGenerate{.request = request()});
}

std::string WizardScreen::t(std::string_view key) const {
    return m_context->localizer->text(key);
}

void WizardScreen::leave(Destination destination) {
    m_next = std::move(destination);
    m_context->controls.exit();
}

}
