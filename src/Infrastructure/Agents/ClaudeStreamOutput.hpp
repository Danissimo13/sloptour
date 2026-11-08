#pragma once

#include "Infrastructure/Agents/IAgentOutput.hpp"
#include "Infrastructure/Json/JsonAccess.hpp"

#include <filesystem>
#include <string>
#include <utility>

namespace sloptour::infrastructure {

class ClaudeStreamOutput final : public IAgentOutput {
public:
    explicit ClaudeStreamOutput(const std::filesystem::path& working_directory)
        : m_prefix(working_directory.string() + "/") {}

    [[nodiscard]] AgentLine parse(std::string_view line) const override;
    [[nodiscard]] application::AgentResult<std::string> reply(std::string_view output) const override;

private:
    [[nodiscard]] std::optional<std::string> activity(const Json& message) const;
    [[nodiscard]] std::string relative(std::string text) const;

    std::string m_prefix;
};

}
