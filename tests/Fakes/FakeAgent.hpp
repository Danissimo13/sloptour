#pragma once

#include "Application/Ports/IAgent.hpp"

#include <cstdint>
#include <functional>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

namespace sloptour::tests {

constexpr std::int64_t FAKE_INPUT_TOKENS = 10;
constexpr std::int64_t FAKE_OUTPUT_TOKENS = 2;

class FakeAgent final : public application::IAgent {
public:
    using Responder = std::function<application::AgentResult<std::string>(const application::AgentTask& task)>;

    explicit FakeAgent(Responder responder)
        : m_responder(std::move(responder)) {}

    [[nodiscard]] std::string_view name() const noexcept override {
        return "fake";
    }

    [[nodiscard]] application::AgentResult<std::string> run(const application::AgentTask& task,
                                                            const application::AgentListener& listener,
                                                            const std::stop_token& /*stop*/) override {
        listener.report("working on " + task.label);
        listener.spend({.input = FAKE_INPUT_TOKENS,
                        .output = FAKE_OUTPUT_TOKENS,
                        .cache_read = 0,
                        .cache_write = 0,
                        .cost_usd = 0.0,
                        .estimated = false});
        const std::scoped_lock lock(m_mutex);
        m_prompts.push_back(task.prompt);
        return m_responder(task);
    }

    [[nodiscard]] std::vector<std::string> prompts() const {
        const std::scoped_lock lock(m_mutex);
        return m_prompts;
    }

private:
    Responder m_responder;
    mutable std::mutex m_mutex;
    std::vector<std::string> m_prompts;
};

}
