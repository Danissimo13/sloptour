#include "Application/Generation/StepIdRegistry.hpp"

#include "Core/Text/Text.hpp"

#include <format>

namespace sloptour::application {

namespace {

constexpr char PREFIX_SEPARATOR = '/';
constexpr std::size_t FIRST_SUFFIX = 2;

constexpr std::string_view FALLBACK_LOCAL_ID = "step";

[[nodiscard]] std::string local_part(std::string_view id) {
    auto local = core::slugify(id);
    return local.empty() ? std::string(FALLBACK_LOCAL_ID) : local;
}

[[nodiscard]] std::string prefixed(std::string_view id, std::string_view prefix) {
    if (prefix.empty()) {
        return local_part(id);
    }
    std::string base(prefix);
    base += PREFIX_SEPARATOR;
    if (id.starts_with(base)) {
        id.remove_prefix(base.size());
    }
    return base + local_part(id);
}

}

void StepIdRegistry::reserve(const domain::Tour& tour) {
    for (const auto& chapter : tour.chapters()) {
        for (const auto& step : chapter.steps()) {
            m_taken.insert(step.id().value());
        }
    }
}

ApplicationResult<std::vector<domain::Step>> StepIdRegistry::claim(const std::vector<domain::Step>& steps,
                                                                   std::string_view prefix) {
    std::vector<domain::Step> claimed;
    claimed.reserve(steps.size());
    for (const auto& step : steps) {
        auto renamed = claim_one(step, prefix);
        if (!renamed) {
            return std::unexpected(renamed.error());
        }
        claimed.push_back(std::move(*renamed));
    }
    return claimed;
}

ApplicationResult<domain::Step> StepIdRegistry::claim_one(const domain::Step& step, std::string_view prefix) {
    auto id = domain::StepId::create(available(prefixed(step.id().value(), prefix)));
    if (!id) {
        return std::unexpected(ApplicationError(id.error()));
    }
    m_taken.insert(id->value());
    return step.with_id(std::move(*id));
}

std::string StepIdRegistry::available(const std::string& base) const {
    auto candidate = base;
    for (std::size_t suffix = FIRST_SUFFIX; m_taken.contains(candidate); ++suffix) {
        candidate = std::format("{}-{}", base, suffix);
    }
    return candidate;
}

}
