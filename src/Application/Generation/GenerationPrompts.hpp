#pragma once

#include "Application/Generation/DepthLevel.hpp"
#include "Application/Generation/DetailLevel.hpp"
#include "Application/Generation/FocusBriefing.hpp"
#include "Application/Generation/GenerationRequest.hpp"
#include "Application/Ports/PromptVariables.hpp"
#include "Application/Ports/RepositoryOverview.hpp"
#include "Domain/Outline/TourOutline.hpp"

#include <cstddef>

namespace sloptour::application {

[[nodiscard]] PromptVariables outline_variables(const GenerationRequest& request, const RepositoryOverview& overview,
                                                const FocusBriefing& briefing, const DepthLevel& depth,
                                                const DetailLevel& detail);

[[nodiscard]] PromptVariables chapter_variables(const PromptVariables& outline_context,
                                                const domain::TourOutline& outline, std::size_t index);

}
