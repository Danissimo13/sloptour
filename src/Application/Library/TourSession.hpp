#pragma once

#include "Application/Ports/IProgressRepository.hpp"
#include "Application/Ports/ITourRepository.hpp"
#include "Application/Ports/PortErrors.hpp"
#include "Domain/Progress/Exchange.hpp"
#include "Domain/Progress/TourProgress.hpp"
#include "Domain/Tour/Step.hpp"
#include "Domain/Tour/StepId.hpp"
#include "Domain/Tour/StepPosition.hpp"
#include "Domain/Tour/Tour.hpp"

#include <memory>
#include <vector>

namespace sloptour::application {

class TourSession {
public:
    struct Dependencies {
        std::shared_ptr<ITourRepository> tours;
        std::shared_ptr<IProgressRepository> progress;
    };

    TourSession(Dependencies dependencies, domain::Tour tour, domain::TourProgress progress);

    [[nodiscard]] const domain::Tour& tour() const noexcept {
        return m_tour;
    }

    [[nodiscard]] const domain::TourProgress& progress() const noexcept {
        return m_progress;
    }

    [[nodiscard]] domain::StepPosition resume_position() const noexcept;
    [[nodiscard]] ApplicationResult<void> visit(domain::StepPosition position);
    [[nodiscard]] ApplicationResult<void> reveal(const domain::StepId& step);
    [[nodiscard]] ApplicationResult<void> record(const domain::StepId& step, domain::Exchange exchange);
    [[nodiscard]] ApplicationResult<void> insert_after(const domain::StepId& step, std::vector<domain::Step> steps);
    [[nodiscard]] ApplicationResult<void> attach_quiz(domain::Quiz quiz);
    [[nodiscard]] ApplicationResult<void> answer(std::size_t question, std::size_t option);
    [[nodiscard]] ApplicationResult<void> restart_quiz();

private:
    [[nodiscard]] ApplicationResult<void> save_progress();

    Dependencies m_dependencies;
    domain::Tour m_tour;
    domain::TourProgress m_progress;
};

}
