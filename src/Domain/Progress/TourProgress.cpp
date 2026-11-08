#include "Domain/Progress/TourProgress.hpp"

namespace sloptour::domain {

void TourProgress::visit(const StepId& step) {
    m_snapshot.current = step;
    m_snapshot.visited.insert(step);
}

void TourProgress::reveal(const StepId& step) {
    m_snapshot.revealed.insert(step);
}

void TourProgress::record(const StepId& step, Exchange exchange) {
    m_snapshot.exchanges[step].push_back(std::move(exchange));
}

void TourProgress::answer(std::size_t question, std::size_t option) {
    m_snapshot.quiz_answers.insert_or_assign(question, option);
}

void TourProgress::reset_quiz() noexcept {
    m_snapshot.quiz_answers.clear();
}

std::optional<std::size_t> TourProgress::quiz_answer(std::size_t question) const noexcept {
    const auto found = m_snapshot.quiz_answers.find(question);
    if (found == m_snapshot.quiz_answers.end()) {
        return std::nullopt;
    }
    return found->second;
}

bool TourProgress::is_visited(const StepId& step) const noexcept {
    return m_snapshot.visited.contains(step);
}

bool TourProgress::is_revealed(const StepId& step) const noexcept {
    return m_snapshot.revealed.contains(step);
}

std::span<const Exchange> TourProgress::exchanges(const StepId& step) const noexcept {
    const auto found = m_snapshot.exchanges.find(step);
    if (found == m_snapshot.exchanges.end()) {
        return {};
    }
    return found->second;
}

std::size_t TourProgress::visited_count_in(const Tour& tour) const noexcept {
    std::size_t count = 0;
    for (const auto& chapter : tour.chapters()) {
        for (const auto& step : chapter.steps()) {
            count += is_visited(step.id()) ? 1U : 0U;
        }
    }
    return count;
}

}
