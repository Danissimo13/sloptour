#pragma once

#include "Infrastructure/Agents/IAgentOutput.hpp"

namespace sloptour::infrastructure {

class TextOutput final : public IAgentOutput {
public:
    [[nodiscard]] AgentLine parse(std::string_view /*line*/) const override {
        return {};
    }

    [[nodiscard]] application::AgentResult<std::string> reply(std::string_view output) const override {
        return std::string(output);
    }
};

}
