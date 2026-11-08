#include "Infrastructure/Agents/AgentOutputs.hpp"

#include "Infrastructure/Agents/ClaudeStreamOutput.hpp"
#include "Infrastructure/Agents/TextOutput.hpp"

#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace sloptour::infrastructure {

namespace {

constexpr std::string_view TEXT = "text";
constexpr std::string_view CLAUDE_STREAM = "claude-stream";

using OutputFactory = std::function<std::shared_ptr<const IAgentOutput>(const std::filesystem::path&)>;

[[nodiscard]] std::vector<std::pair<std::string_view, OutputFactory>> registry() {
    return {
        {TEXT, [](const std::filesystem::path& /*directory*/) { return std::make_shared<const TextOutput>(); }},
        {CLAUDE_STREAM,
         [](const std::filesystem::path& directory) { return std::make_shared<const ClaudeStreamOutput>(directory); }},
    };
}

}

application::AgentResult<std::shared_ptr<const IAgentOutput>>
agent_output(std::string_view name, const std::filesystem::path& working_directory) {
    const auto chosen = name.empty() ? TEXT : name;
    for (const auto& [output, factory] : registry()) {
        if (output == chosen) {
            return factory(working_directory);
        }
    }
    return std::unexpected(application::AgentError("unknown agent output format '" + std::string(name) + "'"));
}

}
