#pragma once

#include "Domain/Quiz/Quiz.hpp"
#include "Domain/Tour/Chapter.hpp"
#include "Domain/Tour/Step.hpp"
#include "Domain/Tour/StepId.hpp"
#include "Domain/Tour/StepPosition.hpp"
#include "Domain/Tour/TourError.hpp"
#include "Domain/Tour/TourId.hpp"
#include "Domain/Tour/TourMetadata.hpp"

#include <cstddef>
#include <optional>
#include <utility>
#include <vector>

namespace sloptour::domain {

class Tour {
public:
    [[nodiscard]] static TourResult<Tour> create(TourId id, TourMetadata metadata, std::vector<Chapter> chapters);

    [[nodiscard]] const TourId& id() const noexcept {
        return m_id;
    }

    [[nodiscard]] const TourMetadata& metadata() const noexcept {
        return m_metadata;
    }

    [[nodiscard]] const std::vector<Chapter>& chapters() const noexcept {
        return m_chapters;
    }

    [[nodiscard]] const std::optional<Quiz>& quiz() const noexcept {
        return m_quiz;
    }

    void attach_quiz(Quiz quiz) {
        m_quiz = std::move(quiz);
    }

    [[nodiscard]] std::size_t step_count() const noexcept;
    [[nodiscard]] std::size_t ordinal_of(StepPosition position) const noexcept;
    [[nodiscard]] const Step& step_at(StepPosition position) const;
    [[nodiscard]] std::optional<StepPosition> find(const StepId& id) const noexcept;
    [[nodiscard]] std::optional<StepPosition> next(StepPosition position) const noexcept;
    [[nodiscard]] std::optional<StepPosition> previous(StepPosition position) const noexcept;
    [[nodiscard]] TourResult<void> insert_steps_after(const StepId& anchor, std::vector<Step> steps);

private:
    Tour(TourId id, TourMetadata metadata, std::vector<Chapter> chapters)
        : m_id(std::move(id))
        , m_metadata(std::move(metadata))
        , m_chapters(std::move(chapters)) {}

    TourId m_id;
    TourMetadata m_metadata;
    std::vector<Chapter> m_chapters;
    std::optional<Quiz> m_quiz;
};

}
