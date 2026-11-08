#pragma once

#include "Domain/Outline/ChapterPlan.hpp"
#include "Domain/Tour/TourError.hpp"

#include <string>
#include <utility>
#include <vector>

namespace sloptour::domain {

class TourOutline {
public:
    [[nodiscard]] static TourResult<TourOutline> create(std::string title, std::string summary,
                                                        std::vector<ChapterPlan> chapters);

    [[nodiscard]] const std::string& title() const noexcept {
        return m_title;
    }

    [[nodiscard]] const std::string& summary() const noexcept {
        return m_summary;
    }

    [[nodiscard]] const std::vector<ChapterPlan>& chapters() const noexcept {
        return m_chapters;
    }

private:
    TourOutline(std::string title, std::string summary, std::vector<ChapterPlan> chapters)
        : m_title(std::move(title))
        , m_summary(std::move(summary))
        , m_chapters(std::move(chapters)) {}

    std::string m_title;
    std::string m_summary;
    std::vector<ChapterPlan> m_chapters;
};

}
