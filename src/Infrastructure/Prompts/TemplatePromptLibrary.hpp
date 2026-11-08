#pragma once

#include "Application/Ports/IPromptLibrary.hpp"

#include <filesystem>
#include <utility>

namespace sloptour::infrastructure {

class TemplatePromptLibrary final : public application::IPromptLibrary {
public:
    explicit TemplatePromptLibrary(std::filesystem::path overrides)
        : m_overrides(std::move(overrides)) {}

    [[nodiscard]] application::PromptResult<std::string>
    render(std::string_view prompt, const application::PromptVariables& variables) const override;

private:
    [[nodiscard]] application::PromptResult<std::string> template_of(std::string_view prompt) const;

    std::filesystem::path m_overrides;
};

}
