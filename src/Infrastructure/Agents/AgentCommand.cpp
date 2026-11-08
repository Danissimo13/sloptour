#include "Infrastructure/Agents/AgentCommand.hpp"

#include <algorithm>

namespace sloptour::infrastructure {

namespace {

constexpr std::string_view MODEL_ARGUMENTS = "{model_arguments}";
constexpr std::string_view MODEL = "{model}";

[[nodiscard]] std::string with_model(std::string argument, std::string_view model) {
    for (auto found = argument.find(MODEL); found != std::string::npos;
         found = argument.find(MODEL, found + model.size())) {
        argument.replace(found, MODEL.size(), model);
    }
    return argument;
}

}

application::AgentResult<std::vector<std::string>> agent_command(const AgentProfile& profile, std::string_view model) {
    const bool supports_models = std::ranges::find(profile.command, MODEL_ARGUMENTS) != profile.command.end();
    if (!model.empty() && !supports_models) {
        return std::unexpected(
            application::AgentError("agent '" + profile.name + "' has no {model_arguments} slot to choose a model"));
    }
    std::vector<std::string> command;
    for (const auto& argument : profile.command) {
        if (argument != MODEL_ARGUMENTS) {
            command.push_back(argument);
            continue;
        }
        if (model.empty()) {
            continue;
        }
        for (const auto& model_argument : profile.model_arguments) {
            command.push_back(with_model(model_argument, model));
        }
    }
    return command;
}

}
