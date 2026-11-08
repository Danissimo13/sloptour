#pragma once

#include "Application/Library/TourSession.hpp"
#include "Application/Library/TourSummary.hpp"
#include "Application/Ports/IProgressRepository.hpp"
#include "Application/Ports/ISourceFiles.hpp"
#include "Application/Ports/ITourRepository.hpp"
#include "Application/Ports/PortErrors.hpp"
#include "Domain/Tour/TourId.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace sloptour::application {

class TourLibrary {
public:
    struct Dependencies {
        std::shared_ptr<ITourRepository> tours;
        std::shared_ptr<IProgressRepository> progress;
        std::shared_ptr<const ISourceFiles> sources;
    };

    explicit TourLibrary(Dependencies dependencies)
        : m_dependencies(std::move(dependencies)) {}

    [[nodiscard]] ApplicationResult<std::vector<TourSummary>> summaries() const;
    [[nodiscard]] ApplicationResult<TourSession> open(const domain::TourId& id) const;
    [[nodiscard]] ApplicationResult<std::vector<std::string>> source(std::string_view path) const;

private:
    [[nodiscard]] ApplicationResult<TourSummary> summary_of(const domain::TourId& id) const;

    Dependencies m_dependencies;
};

}
