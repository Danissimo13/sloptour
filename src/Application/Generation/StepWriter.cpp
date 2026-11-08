#include "Application/Generation/StepWriter.hpp"

#include <iterator>

namespace sloptour::application {

ApplicationResult<std::vector<domain::Step>> StepWriter::write(std::string_view prompt,
                                                               const PromptVariables& variables,
                                                               const std::string& label, const AgentListener& listener,
                                                               const std::stop_token& stop) const {
    auto rendered = m_dependencies.prompts->render(prompt, variables);
    if (!rendered) {
        return std::unexpected(ApplicationError(rendered.error()));
    }
    const auto& decoder = *m_dependencies.decoder;
    auto steps = m_dependencies.dialogue->request(
        AgentTask{.label = label, .prompt = std::move(*rendered)},
        [&decoder](std::string_view reply) { return decoder.decode_steps(reply); }, listener, stop);
    if (steps && steps->size() > m_dependencies.max_steps) {
        steps->erase(std::next(steps->begin(), static_cast<std::ptrdiff_t>(m_dependencies.max_steps)), steps->end());
    }
    return steps;
}

}
