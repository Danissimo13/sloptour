#include "Infrastructure/Agents/CommandAgent.hpp"

#include "Core/Text/Text.hpp"
#include "Infrastructure/Agents/UsageMeter.hpp"

#include <algorithm>
#include <cstdint>
#include <format>

namespace sloptour::infrastructure {

namespace {

constexpr std::size_t ERROR_TAIL_LENGTH = 600;
constexpr std::size_t CHARACTERS_PER_TOKEN = 4;

[[nodiscard]] application::TokenUsage estimated_usage(std::string_view prompt, std::string_view reply) {
    return {.input = static_cast<std::int64_t>(prompt.size() / CHARACTERS_PER_TOKEN),
            .output = static_cast<std::int64_t>(reply.size() / CHARACTERS_PER_TOKEN),
            .cache_read = 0,
            .cache_write = 0,
            .cost_usd = 0.0,
            .estimated = true};
}

[[nodiscard]] std::string tail_of(std::string_view text) {
    const auto trimmed = core::trim(text);
    return std::string(trimmed.substr(trimmed.size() - std::min(trimmed.size(), ERROR_TAIL_LENGTH)));
}

}

application::AgentResult<std::string> CommandAgent::run(const application::AgentTask& task,
                                                        const application::AgentListener& listener,
                                                        const std::stop_token& stop) {
    const auto& profile = m_dependencies.profile;
    const auto& output = *m_dependencies.output;
    UsageMeter meter;
    const ProcessCommand command{.arguments = m_dependencies.command,
                                 .input = task.prompt,
                                 .working_directory = m_dependencies.working_directory,
                                 .timeout = profile.timeout};
    const ProcessListeners listeners{.output =
                                         [&output, &listener, &meter](std::string_view text) {
                                             const auto line = output.parse(text);
                                             if (line.activity) {
                                                 listener.report(*line.activity);
                                             }
                                             if (line.usage) {
                                                 listener.spend(meter.record(*line.usage));
                                             }
                                         },
                                     .errors = [&listener](std::string_view text) { listener.report(text); }};
    auto outcome = m_dependencies.runner->run(command, listeners, stop);
    if (!outcome) {
        return std::unexpected(application::AgentError(outcome.error()).located_at(profile.name));
    }
    if (outcome->status != 0) {
        return std::unexpected(application::AgentError(
            std::format("{} exited with status {}: {}", profile.name, outcome->status, tail_of(outcome->errors))));
    }
    auto reply = output.reply(outcome->output);
    if (!reply) {
        return std::unexpected(reply.error().located_at(profile.name));
    }
    if (core::is_blank(*reply)) {
        return std::unexpected(application::AgentError(profile.name + " returned an empty reply"));
    }
    if (meter.empty()) {
        listener.spend(estimated_usage(task.prompt, *reply));
    }
    return reply;
}

}
