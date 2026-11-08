#pragma once

#include "Application/Ports/AgentListener.hpp"
#include "Application/Ports/AgentTask.hpp"
#include "Application/Ports/IAgent.hpp"
#include "Application/Ports/IPromptLibrary.hpp"
#include "Application/Ports/PortErrors.hpp"

#include <cstddef>
#include <memory>
#include <stop_token>
#include <string>
#include <string_view>
#include <utility>

namespace sloptour::application {

class AgentDialogue {
public:
    AgentDialogue(std::shared_ptr<IAgent> agent, std::shared_ptr<const IPromptLibrary> prompts,
                  std::size_t repair_attempts)
        : m_agent(std::move(agent))
        , m_prompts(std::move(prompts))
        , m_repair_attempts(repair_attempts) {}

    template <typename TDecode>
    [[nodiscard]] auto request(const AgentTask& task, TDecode decode, const AgentListener& listener,
                               const std::stop_token& stop) const
        -> ApplicationResult<typename decltype(decode(std::string_view{}))::value_type> {
        auto current = task;
        for (std::size_t attempt = 0;; ++attempt) {
            auto reply = ask(current, listener, stop);
            if (!reply) {
                return std::unexpected(reply.error());
            }
            auto decoded = decode(std::string_view(*reply));
            if (decoded) {
                return std::move(*decoded);
            }
            if (attempt >= m_repair_attempts || stop.stop_requested()) {
                return std::unexpected(ApplicationError(decoded.error()).located_at(task.label));
            }
            auto repair = repaired(task, *reply, decoded.error().message(), listener);
            if (!repair) {
                return std::unexpected(repair.error());
            }
            current = std::move(*repair);
        }
    }

    [[nodiscard]] ApplicationResult<std::string> ask(const AgentTask& task, const AgentListener& listener,
                                                     const std::stop_token& stop) const;

    [[nodiscard]] std::string_view agent_name() const noexcept {
        return m_agent->name();
    }

private:
    [[nodiscard]] ApplicationResult<AgentTask> repaired(const AgentTask& original, std::string_view reply,
                                                        std::string_view problem, const AgentListener& listener) const;

    std::shared_ptr<IAgent> m_agent;
    std::shared_ptr<const IPromptLibrary> m_prompts;
    std::size_t m_repair_attempts;
};

}
