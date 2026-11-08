#pragma once

#include "Domain/Tour/Step.hpp"
#include "Domain/Tour/TourError.hpp"

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace sloptour::domain {

class Chapter {
public:
    [[nodiscard]] static TourResult<Chapter> create(std::string title, std::string summary, std::vector<Step> steps);

    [[nodiscard]] const std::string& title() const noexcept {
        return m_title;
    }

    [[nodiscard]] const std::string& summary() const noexcept {
        return m_summary;
    }

    [[nodiscard]] const std::vector<Step>& steps() const noexcept {
        return m_steps;
    }

    [[nodiscard]] Chapter with_steps_inserted_after(std::size_t index, std::vector<Step> steps) const;

private:
    Chapter(std::string title, std::string summary, std::vector<Step> steps)
        : m_title(std::move(title))
        , m_summary(std::move(summary))
        , m_steps(std::move(steps)) {}

    std::string m_title;
    std::string m_summary;
    std::vector<Step> m_steps;
};

}
