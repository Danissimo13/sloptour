#pragma once

#include "Application/Ports/IAgentProvider.hpp"

#include <memory>
#include <utility>

namespace sloptour::tests {

class FakeAgentProvider final : public application::IAgentProvider {
public:
    explicit FakeAgentProvider(std::shared_ptr<application::IAgent> agent)
        : m_agent(std::move(agent)) {}

    [[nodiscard]] std::vector<application::AgentDescriptor> agents() const override {
        return {
            {.name = "fake", .description = "scripted", .executable = "fake", .models = {"big"}, .installed = true}};
    }

    [[nodiscard]] std::string_view default_agent() const noexcept override {
        return "fake";
    }

    [[nodiscard]] application::AgentResult<std::shared_ptr<application::IAgent>>
    find(const application::AgentChoice& choice) const override {
        if (choice.name != "fake") {
            return std::unexpected(application::AgentError("unknown agent"));
        }
        return m_agent;
    }

private:
    std::shared_ptr<application::IAgent> m_agent;
};

}
