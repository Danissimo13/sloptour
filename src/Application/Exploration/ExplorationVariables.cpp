#include "Application/Exploration/ExplorationVariables.hpp"

#include "Application/Exploration/StepBrief.hpp"
#include "Application/Preferences/LanguageInstruction.hpp"

namespace sloptour::application {

ApplicationResult<PromptVariables> exploration_variables(const ExplorationRequest& request) {
    const auto position = request.tour.find(request.step);
    if (!position) {
        return std::unexpected(ApplicationError("step '" + request.step.value() + "' is not part of the tour"));
    }
    const auto& chapter = request.tour.chapters().at(position->chapter);
    return PromptVariables{
        {"tour_title", request.tour.metadata().title},
        {"tour_summary", request.tour.metadata().summary},
        {"tour_scope", request.tour.metadata().scope},
        {"chapter_title", chapter.title()},
        {"step_id", request.step.value()},
        {"step", brief_of(request.tour.step_at(*position))},
        {"history", history_of(request.history)},
        {"question", request.question},
        {"language", language_of(request.language)},
    };
}

}
