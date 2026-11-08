#include "Application/Generation/FocusBriefer.hpp"

#include "Application/Generation/PromptNames.hpp"
#include "Core/Text/Text.hpp"

#include <variant>

namespace sloptour::application {

namespace {

constexpr std::string_view WHOLE_REPOSITORY = "whole repository";
constexpr std::string_view NOTHING = "(none)";
constexpr std::string_view ONBOARDING_LABEL = "onboarding";
constexpr std::string_view RECAP_LABEL = "recap";
constexpr std::string_view FUNCTIONAL_LABEL = "functional";
constexpr std::size_t SCOPE_LENGTH = 80;

[[nodiscard]] std::string or_nothing(const std::string& text) {
    return text.empty() ? std::string(NOTHING) : text;
}

}

ApplicationResult<FocusBriefing> FocusBriefer::brief(const TourFocus& focus) const {
    return std::visit(*this, focus);
}

ApplicationResult<FocusBriefing> FocusBriefer::operator()(const OnboardingFocus& /*focus*/) const {
    return FocusBriefing{
        .kind = domain::TourKind::Onboarding,
        .label = ONBOARDING_LABEL,
        .outline_prompt = prompt_names::ONBOARDING_OUTLINE,
        .chapter_prompt = prompt_names::ONBOARDING_CHAPTER,
        .scope = std::string(WHOLE_REPOSITORY),
        .variables = {},
    };
}

ApplicationResult<FocusBriefing> FocusBriefer::operator()(const RecapFocus& focus) const {
    auto digest = m_inspector->changes(focus.scope);
    if (!digest) {
        return std::unexpected(ApplicationError(digest.error()));
    }
    return FocusBriefing{
        .kind = domain::TourKind::Recap,
        .label = RECAP_LABEL,
        .outline_prompt = prompt_names::RECAP_OUTLINE,
        .chapter_prompt = prompt_names::RECAP_CHAPTER,
        .scope = digest->range,
        .variables =
            {
                {"change_range", digest->range},
                {"commit_log", or_nothing(digest->commits)},
                {"diff_stat", or_nothing(digest->diff_stat)},
                {"working_tree", or_nothing(digest->working_tree)},
            },
    };
}

ApplicationResult<FocusBriefing> FocusBriefer::operator()(const FunctionalFocus& focus) const {
    const auto functionality = core::trim(focus.functionality);
    if (functionality.empty()) {
        return std::unexpected(ApplicationError("describe the functionality the tour should follow"));
    }
    return FocusBriefing{
        .kind = domain::TourKind::Functional,
        .label = FUNCTIONAL_LABEL,
        .outline_prompt = prompt_names::FUNCTIONAL_OUTLINE,
        .chapter_prompt = prompt_names::FUNCTIONAL_CHAPTER,
        .scope = std::string(functionality.substr(0, SCOPE_LENGTH)),
        .variables = {{"functionality", std::string(functionality)}},
    };
}

}
