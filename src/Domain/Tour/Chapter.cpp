#include "Domain/Tour/Chapter.hpp"

#include "Core/Text/Text.hpp"

#include <iterator>

namespace sloptour::domain {

TourResult<Chapter> Chapter::create(std::string title, std::string summary, std::vector<Step> steps) {
    if (core::is_blank(title)) {
        return std::unexpected(TourError("chapter has no title"));
    }
    if (steps.empty()) {
        return std::unexpected(TourError("chapter '" + title + "' has no steps"));
    }
    return Chapter(std::move(title), std::move(summary), std::move(steps));
}

Chapter Chapter::with_steps_inserted_after(std::size_t index, std::vector<Step> steps) const {
    auto combined = m_steps;
    const auto position = std::next(combined.begin(), static_cast<std::ptrdiff_t>(index + 1));
    combined.insert(position, std::make_move_iterator(steps.begin()), std::make_move_iterator(steps.end()));
    return {m_title, m_summary, std::move(combined)};
}

}
