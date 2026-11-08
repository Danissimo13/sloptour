#pragma once

#include "Application/Ports/PortErrors.hpp"
#include "Application/Ports/PromptVariables.hpp"

#include <string>
#include <string_view>

namespace sloptour::application {

class IPromptLibrary {
public:
    IPromptLibrary() = default;
    virtual ~IPromptLibrary() = default;
    IPromptLibrary(const IPromptLibrary&) = delete;
    IPromptLibrary& operator=(const IPromptLibrary&) = delete;
    IPromptLibrary(IPromptLibrary&&) = delete;
    IPromptLibrary& operator=(IPromptLibrary&&) = delete;

    [[nodiscard]] virtual PromptResult<std::string> render(std::string_view prompt,
                                                           const PromptVariables& variables) const = 0;
};

}
