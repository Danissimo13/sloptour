#include "Application/Generation/GenerationPrompts.hpp"

#include "Application/Preferences/LanguageInstruction.hpp"
#include "Core/Text/Text.hpp"

#include <format>

namespace sloptour::application {

namespace {

constexpr std::string_view NO_HINT = "No special focus requested.";
constexpr std::string_view NOT_VERSIONED = "(not a git repository)";
constexpr std::string_view LIST_SEPARATOR = ", ";

[[nodiscard]] std::string described_outline(const domain::TourOutline& outline) {
    std::string text;
    std::size_t number = 1;
    for (const auto& plan : outline.chapters()) {
        text += std::format("{}. [{}] {} - {}\n", number++, plan.id, plan.title, plan.goal);
    }
    return text;
}

[[nodiscard]] std::string hint_of(const GenerationRequest& request) {
    if (core::is_blank(request.hint)) {
        return std::string(NO_HINT);
    }
    return std::string(core::trim(request.hint));
}

}

PromptVariables outline_variables(const GenerationRequest& request, const RepositoryOverview& overview,
                                  const FocusBriefing& briefing, const DepthLevel& depth, const DetailLevel& detail) {
    auto variables = briefing.variables;
    variables.insert_or_assign("repository", overview.name);
    variables.insert_or_assign("branch", overview.versioned ? overview.branch : std::string(NOT_VERSIONED));
    variables.insert_or_assign("head", overview.versioned ? overview.head : std::string(NOT_VERSIONED));
    variables.insert_or_assign("hint", hint_of(request));
    variables.insert_or_assign("language", language_of(request.language));
    variables.insert_or_assign("min_chapters", std::to_string(depth.min_chapters));
    variables.insert_or_assign("max_chapters", std::to_string(depth.max_chapters));
    variables.insert_or_assign("max_steps", std::to_string(depth.max_steps));
    variables.insert_or_assign("depth_guidance", depth.guidance);
    variables.insert_or_assign("detail_guidance", detail.guidance);
    return variables;
}

PromptVariables chapter_variables(const PromptVariables& outline_context, const domain::TourOutline& outline,
                                  std::size_t index) {
    const auto& plan = outline.chapters().at(index);
    auto variables = outline_context;
    variables.insert_or_assign("tour_title", outline.title());
    variables.insert_or_assign("tour_summary", outline.summary());
    variables.insert_or_assign("outline", described_outline(outline));
    variables.insert_or_assign("chapter_number", std::to_string(index + 1));
    variables.insert_or_assign("chapter_id", plan.id);
    variables.insert_or_assign("chapter_title", plan.title);
    variables.insert_or_assign("chapter_goal", plan.goal);
    variables.insert_or_assign("focus_paths", core::join(plan.focus_paths, LIST_SEPARATOR));
    return variables;
}

}
