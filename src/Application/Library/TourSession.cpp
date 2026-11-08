#include "Application/Library/TourSession.hpp"

namespace sloptour::application {

TourSession::TourSession(Dependencies dependencies, domain::Tour tour, domain::TourProgress progress)
    : m_dependencies(std::move(dependencies))
    , m_tour(std::move(tour))
    , m_progress(std::move(progress)) {}

domain::StepPosition TourSession::resume_position() const noexcept {
    if (const auto& current = m_progress.current()) {
        if (const auto position = m_tour.find(*current)) {
            return *position;
        }
    }
    return {};
}

ApplicationResult<void> TourSession::visit(domain::StepPosition position) {
    m_progress.visit(m_tour.step_at(position).id());
    return save_progress();
}

ApplicationResult<void> TourSession::reveal(const domain::StepId& step) {
    m_progress.reveal(step);
    return save_progress();
}

ApplicationResult<void> TourSession::record(const domain::StepId& step, domain::Exchange exchange) {
    m_progress.record(step, std::move(exchange));
    return save_progress();
}

ApplicationResult<void> TourSession::insert_after(const domain::StepId& step, std::vector<domain::Step> steps) {
    if (auto inserted = m_tour.insert_steps_after(step, std::move(steps)); !inserted) {
        return std::unexpected(ApplicationError(inserted.error()));
    }
    return lifted(m_dependencies.tours->save(m_tour));
}

ApplicationResult<void> TourSession::attach_quiz(domain::Quiz quiz) {
    m_tour.attach_quiz(std::move(quiz));
    m_progress.reset_quiz();
    if (auto saved = save_progress(); !saved) {
        return saved;
    }
    return lifted(m_dependencies.tours->save(m_tour));
}

ApplicationResult<void> TourSession::answer(std::size_t question, std::size_t option) {
    m_progress.answer(question, option);
    return save_progress();
}

ApplicationResult<void> TourSession::restart_quiz() {
    m_progress.reset_quiz();
    return save_progress();
}

ApplicationResult<void> TourSession::save_progress() {
    return lifted(m_dependencies.progress->save(m_progress));
}

}
