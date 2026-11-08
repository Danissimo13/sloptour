#include "Application/Generation/AgentDialogue.hpp"

#include "Application/Generation/PromptNames.hpp"

namespace sloptour::application {

ApplicationResult<std::string> AgentDialogue::ask(const AgentTask& task, const AgentListener& listener,
                                                  const std::stop_token& stop) const {
    return lifted(m_agent->run(task, listener, stop));
}

ApplicationResult<AgentTask> AgentDialogue::repaired(const AgentTask& original, std::string_view reply,
                                                     std::string_view problem, const AgentListener& listener) const {
    listener.report("reply was not valid, asking the agent to fix it: " + std::string(problem));
    auto prompt = m_prompts->render(prompt_names::REPAIR, {
                                                              {"original_prompt", original.prompt},
                                                              {"previous_reply", std::string(reply)},
                                                              {"problem", std::string(problem)},
                                                          });
    if (!prompt) {
        return std::unexpected(ApplicationError(prompt.error()));
    }
    return AgentTask{.label = original.label, .prompt = std::move(*prompt)};
}

}
