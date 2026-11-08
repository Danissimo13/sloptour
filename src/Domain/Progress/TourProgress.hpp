#pragma once

#include "Domain/Progress/Exchange.hpp"
#include "Domain/Progress/ProgressSnapshot.hpp"
#include "Domain/Tour/StepId.hpp"
#include "Domain/Tour/Tour.hpp"
#include "Domain/Tour/TourId.hpp"

#include <cstddef>
#include <optional>
#include <span>
#include <utility>

namespace sloptour::domain {

class TourProgress {
public:
    explicit TourProgress(TourId tour)
        : m_tour(std::move(tour)) {}

    TourProgress(TourId tour, ProgressSnapshot snapshot)
        : m_tour(std::move(tour))
        , m_snapshot(std::move(snapshot)) {}

    [[nodiscard]] const TourId& tour() const noexcept {
        return m_tour;
    }

    [[nodiscard]] const ProgressSnapshot& snapshot() const noexcept {
        return m_snapshot;
    }

    [[nodiscard]] const std::optional<StepId>& current() const noexcept {
        return m_snapshot.current;
    }

    void visit(const StepId& step);
    void reveal(const StepId& step);
    void record(const StepId& step, Exchange exchange);
    void answer(std::size_t question, std::size_t option);
    void reset_quiz() noexcept;

    [[nodiscard]] std::optional<std::size_t> quiz_answer(std::size_t question) const noexcept;

    [[nodiscard]] bool is_visited(const StepId& step) const noexcept;
    [[nodiscard]] bool is_revealed(const StepId& step) const noexcept;
    [[nodiscard]] std::span<const Exchange> exchanges(const StepId& step) const noexcept;
    [[nodiscard]] std::size_t visited_count_in(const Tour& tour) const noexcept;

private:
    TourId m_tour;
    ProgressSnapshot m_snapshot;
};

}
