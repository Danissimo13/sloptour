#pragma once

#include "Application/Generation/AgentDialogue.hpp"
#include "Application/Ports/AgentListener.hpp"
#include "Application/Ports/IPromptLibrary.hpp"
#include "Application/Ports/IReplyDecoder.hpp"
#include "Application/Ports/PortErrors.hpp"
#include "Application/Ports/PromptVariables.hpp"
#include "Domain/Tour/Step.hpp"

#include <cstddef>
#include <memory>
#include <stop_token>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace sloptour::application {

class StepWriter {
public:
    struct Dependencies {
        std::shared_ptr<const AgentDialogue> dialogue;
        std::shared_ptr<const IPromptLibrary> prompts;
        std::shared_ptr<const IReplyDecoder> decoder;
        std::size_t max_steps = 0;
    };

    explicit StepWriter(Dependencies dependencies)
        : m_dependencies(std::move(dependencies)) {}

    [[nodiscard]] ApplicationResult<std::vector<domain::Step>>
    write(std::string_view prompt, const PromptVariables& variables, const std::string& label,
          const AgentListener& listener, const std::stop_token& stop) const;

private:
    Dependencies m_dependencies;
};

}
