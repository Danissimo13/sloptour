#include "Domain/Tour/Tour.hpp"

#include "Core/Errors/Panic.hpp"
#include "Core/Text/Text.hpp"

#include <set>

namespace sloptour::domain {

namespace {

[[nodiscard]] std::optional<StepId> first_duplicate(const std::vector<Chapter>& chapters,
                                                    const std::vector<Step>& extra) {
    std::set<StepId> seen;
    for (const auto& chapter : chapters) {
        for (const auto& step : chapter.steps()) {
            if (!seen.insert(step.id()).second) {
                return step.id();
            }
        }
    }
    for (const auto& step : extra) {
        if (!seen.insert(step.id()).second) {
            return step.id();
        }
    }
    return std::nullopt;
}

}

TourResult<Tour> Tour::create(TourId id, TourMetadata metadata, std::vector<Chapter> chapters) {
    if (core::is_blank(metadata.title)) {
        return std::unexpected(TourError("tour has no title"));
    }
    if (chapters.empty()) {
        return std::unexpected(TourError("tour has no chapters"));
    }
    if (const auto duplicate = first_duplicate(chapters, {})) {
        return std::unexpected(TourError("duplicate step id '" + duplicate->value() + "'"));
    }
    return Tour(std::move(id), std::move(metadata), std::move(chapters));
}

std::size_t Tour::step_count() const noexcept {
    std::size_t count = 0;
    for (const auto& chapter : m_chapters) {
        count += chapter.steps().size();
    }
    return count;
}

std::size_t Tour::ordinal_of(StepPosition position) const noexcept {
    std::size_t ordinal = position.step;
    for (std::size_t index = 0; index < position.chapter && index < m_chapters.size(); ++index) {
        ordinal += m_chapters.at(index).steps().size();
    }
    return ordinal;
}

const Step& Tour::step_at(StepPosition position) const {
    if (position.chapter >= m_chapters.size() || position.step >= m_chapters.at(position.chapter).steps().size()) {
        core::panic("step position is outside of the tour");
    }
    return m_chapters.at(position.chapter).steps().at(position.step);
}

std::optional<StepPosition> Tour::find(const StepId& id) const noexcept {
    for (std::size_t chapter = 0; chapter < m_chapters.size(); ++chapter) {
        const auto& steps = m_chapters.at(chapter).steps();
        for (std::size_t step = 0; step < steps.size(); ++step) {
            if (steps.at(step).id() == id) {
                return StepPosition{.chapter = chapter, .step = step};
            }
        }
    }
    return std::nullopt;
}

std::optional<StepPosition> Tour::next(StepPosition position) const noexcept {
    if (position.step + 1 < m_chapters.at(position.chapter).steps().size()) {
        return StepPosition{.chapter = position.chapter, .step = position.step + 1};
    }
    if (position.chapter + 1 < m_chapters.size()) {
        return StepPosition{.chapter = position.chapter + 1, .step = 0};
    }
    return std::nullopt;
}

std::optional<StepPosition> Tour::previous(StepPosition position) const noexcept {
    if (position.step > 0) {
        return StepPosition{.chapter = position.chapter, .step = position.step - 1};
    }
    if (position.chapter > 0) {
        const auto chapter = position.chapter - 1;
        return StepPosition{.chapter = chapter, .step = m_chapters.at(chapter).steps().size() - 1};
    }
    return std::nullopt;
}

TourResult<void> Tour::insert_steps_after(const StepId& anchor, std::vector<Step> steps) {
    const auto position = find(anchor);
    if (!position) {
        return std::unexpected(TourError("step '" + anchor.value() + "' is not part of the tour"));
    }
    if (const auto duplicate = first_duplicate(m_chapters, steps)) {
        return std::unexpected(TourError("duplicate step id '" + duplicate->value() + "'"));
    }
    auto& chapter = m_chapters.at(position->chapter);
    chapter = chapter.with_steps_inserted_after(position->step, std::move(steps));
    return {};
}

}
