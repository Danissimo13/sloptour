#include "Application/Exploration/AskAboutStep.hpp"

#include "Application/Exploration/ExplorationVariables.hpp"
#include "Application/Generation/PromptNames.hpp"
#include "Core/Text/Text.hpp"

namespace sloptour::application {

namespace {

constexpr std::string_view ASK_LABEL = "question";

}

ApplicationResult<domain::Exchange> AskAboutStep::execute(const ExplorationRequest& request,
                                                          const AgentListener& listener,
                                                          const std::stop_token& stop) const {
    if (core::is_blank(request.question)) {
        return std::unexpected(ApplicationError("question is empty"));
    }
    auto agent = m_agents->find(request.agent);
    if (!agent) {
        return std::unexpected(ApplicationError(agent.error()));
    }
    auto variables = exploration_variables(request);
    if (!variables) {
        return std::unexpected(variables.error());
    }
    auto prompt = m_prompts->render(prompt_names::ASK, *variables);
    if (!prompt) {
        return std::unexpected(ApplicationError(prompt.error()));
    }
    auto answer = (*agent)->run({.label = std::string(ASK_LABEL), .prompt = std::move(*prompt)}, listener, stop);
    if (!answer) {
        return std::unexpected(ApplicationError(answer.error()));
    }
    return domain::Exchange{.question = request.question, .answer = std::string(core::trim(*answer))};
}

}
