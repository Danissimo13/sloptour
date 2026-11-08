#include "Application/Exploration/DeepenStep.hpp"

#include "Application/Exploration/ExplorationVariables.hpp"
#include "Application/Generation/AgentDialogue.hpp"
#include "Application/Generation/PromptNames.hpp"
#include "Application/Generation/StepIdRegistry.hpp"
#include "Application/Generation/StepWriter.hpp"

namespace sloptour::application {

namespace {

constexpr std::string_view DEEPEN_LABEL = "deeper";

}

ApplicationResult<std::vector<domain::Step>> DeepenStep::execute(const ExplorationRequest& request,
                                                                 const AgentListener& listener,
                                                                 const std::stop_token& stop) const {
    auto agent = m_dependencies.agents->find(request.agent);
    if (!agent) {
        return std::unexpected(ApplicationError(agent.error()));
    }
    auto variables = exploration_variables(request);
    if (!variables) {
        return std::unexpected(variables.error());
    }
    auto depth = m_dependencies.settings.depth(request.tour.metadata().depth);
    auto detail = m_dependencies.settings.detail(request.tour.metadata().detail);
    if (!depth || !detail) {
        return std::unexpected(!depth ? depth.error() : detail.error());
    }
    variables->insert_or_assign("max_steps", std::to_string(depth->deepen_steps));
    variables->insert_or_assign("detail_guidance", detail->guidance);
    const StepWriter writer({
        .dialogue = std::make_shared<const AgentDialogue>(*agent, m_dependencies.prompts,
                                                          m_dependencies.settings.repair_attempts),
        .prompts = m_dependencies.prompts,
        .decoder = m_dependencies.decoder,
        .max_steps = depth->deepen_steps,
    });
    auto steps = writer.write(prompt_names::DEEPEN, *variables, std::string(DEEPEN_LABEL), listener, stop);
    if (!steps) {
        return std::unexpected(steps.error());
    }
    StepIdRegistry registry;
    registry.reserve(request.tour);
    return registry.claim(*steps, request.step.value());
}

}
