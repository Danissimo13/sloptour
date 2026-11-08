#pragma once

#include "Application/Ports/IPromptLibrary.hpp"

#include <string>

namespace sloptour::tests {

class FakePromptLibrary final : public application::IPromptLibrary {
public:
    [[nodiscard]] application::PromptResult<std::string>
    render(std::string_view prompt, const application::PromptVariables& variables) const override {
        std::string rendered(prompt);
        for (const auto& [name, value] : variables) {
            if (name == "chapter_id" || name == "step_id" || name == "problem" || name == "questions") {
                rendered.append("|").append(name).append("=").append(value);
            }
        }
        return rendered;
    }
};

}
