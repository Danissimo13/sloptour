#include "Application/Library/TourLibrary.hpp"

#include <algorithm>

namespace sloptour::application {

ApplicationResult<std::vector<TourSummary>> TourLibrary::summaries() const {
    auto ids = m_dependencies.tours->list();
    if (!ids) {
        return std::unexpected(ApplicationError(ids.error()));
    }
    std::vector<TourSummary> found;
    for (const auto& id : *ids) {
        if (auto summary = summary_of(id)) {
            found.push_back(std::move(*summary));
        }
    }
    std::ranges::sort(found, std::ranges::greater{},
                      [](const TourSummary& summary) { return summary.metadata.created_at; });
    return found;
}

ApplicationResult<TourSession> TourLibrary::open(const domain::TourId& id) const {
    auto tour = m_dependencies.tours->load(id);
    if (!tour) {
        return std::unexpected(ApplicationError(tour.error()));
    }
    auto progress = m_dependencies.progress->load(id);
    if (!progress) {
        return std::unexpected(ApplicationError(progress.error()));
    }
    return TourSession({.tours = m_dependencies.tours, .progress = m_dependencies.progress}, std::move(*tour),
                       std::move(*progress));
}

ApplicationResult<std::vector<std::string>> TourLibrary::source(std::string_view path) const {
    return lifted(m_dependencies.sources->read_lines(path));
}

ApplicationResult<TourSummary> TourLibrary::summary_of(const domain::TourId& id) const {
    auto session = open(id);
    if (!session) {
        return std::unexpected(session.error());
    }
    const auto& tour = session->tour();
    return TourSummary{.id = tour.id(),
                       .metadata = tour.metadata(),
                       .chapters = tour.chapters().size(),
                       .steps = tour.step_count(),
                       .visited = session->progress().visited_count_in(tour),
                       .has_quiz = tour.quiz().has_value()};
}

}
