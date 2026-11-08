#pragma once

#include "Application/Ports/AgentDescriptor.hpp"
#include "Application/Ports/CommitSummary.hpp"
#include "Domain/Tour/TourKind.hpp"
#include "Terminal/Shell/Destination.hpp"
#include "Terminal/Shell/TerminalContext.hpp"

#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace sloptour::terminal {

class WizardScreen {
public:
    WizardScreen(std::shared_ptr<const TerminalContext> context, domain::TourKind kind);

    [[nodiscard]] Destination run();

private:
    void build_controls();
    [[nodiscard]] ftxui::Component build();
    [[nodiscard]] ftxui::Element render() const;
    [[nodiscard]] ftxui::Element field(std::string_view label, std::string_view help, ftxui::Element control) const;
    [[nodiscard]] application::GenerationRequest request() const;
    [[nodiscard]] application::TourFocus focus() const;
    [[nodiscard]] ftxui::Element render_since(bool focused) const;
    [[nodiscard]] bool step_since(const ftxui::Event& event);
    [[nodiscard]] ftxui::Elements kind_fields() const;
    [[nodiscard]] std::string t(std::string_view key) const;
    void sync_models();
    void submit();
    void leave(Destination destination);

    std::shared_ptr<const TerminalContext> m_context;
    domain::TourKind m_kind;
    std::vector<application::AgentDescriptor> m_agents;
    std::vector<std::string> m_agent_labels;
    int m_agent_index = 0;
    std::vector<std::string> m_model_labels;
    int m_model_index = 0;
    std::vector<std::string> m_depth_labels;
    std::vector<std::string> m_depth_names;
    int m_depth_index = 0;
    std::vector<std::string> m_detail_labels;
    std::vector<std::string> m_detail_names;
    int m_detail_index = 0;
    std::vector<application::CommitSummary> m_commits;
    std::vector<std::string> m_commit_labels;
    int m_commit_index = 0;
    bool m_include_working_tree = true;
    std::string m_hint;
    std::string m_functionality;
    std::string m_name;
    std::optional<std::string> m_problem;
    ftxui::Component m_agent_menu;
    ftxui::Component m_model_menu;
    ftxui::Component m_depth_menu;
    ftxui::Component m_detail_menu;
    ftxui::Component m_commit_menu;
    ftxui::Component m_working_tree;
    ftxui::Component m_hint_input;
    ftxui::Component m_functionality_input;
    ftxui::Component m_name_input;
    ftxui::Component m_buttons;
    Destination m_next = GoHome{};
};

}
