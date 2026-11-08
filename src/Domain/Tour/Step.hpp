#pragma once

#include "Domain/Tour/Checkpoint.hpp"
#include "Domain/Tour/CodeLocation.hpp"
#include "Domain/Tour/StepDraft.hpp"
#include "Domain/Tour/StepId.hpp"
#include "Domain/Tour/TourError.hpp"

#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace sloptour::domain {

class Step {
public:
    [[nodiscard]] static TourResult<Step> create(StepDraft draft);

    [[nodiscard]] const StepId& id() const noexcept {
        return m_draft.id;
    }

    [[nodiscard]] const std::string& title() const noexcept {
        return m_draft.title;
    }

    [[nodiscard]] const std::string& body() const noexcept {
        return m_draft.body;
    }

    [[nodiscard]] const std::vector<CodeLocation>& locations() const noexcept {
        return m_draft.locations;
    }

    [[nodiscard]] const std::vector<std::string>& commands() const noexcept {
        return m_draft.commands;
    }

    [[nodiscard]] const std::optional<Checkpoint>& checkpoint() const noexcept {
        return m_draft.checkpoint;
    }

    [[nodiscard]] Step with_id(StepId id) const {
        auto draft = m_draft;
        draft.id = std::move(id);
        return Step(std::move(draft));
    }

private:
    explicit Step(StepDraft draft)
        : m_draft(std::move(draft)) {}

    StepDraft m_draft;
};

}
