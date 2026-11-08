#pragma once

#include "Domain/Tour/LineRange.hpp"
#include "Domain/Tour/TourError.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace sloptour::domain {

class CodeLocation {
public:
    [[nodiscard]] static TourResult<CodeLocation> create(std::string_view path, std::optional<LineRange> lines,
                                                         std::string note);

    [[nodiscard]] const std::string& path() const noexcept {
        return m_path;
    }

    [[nodiscard]] const std::optional<LineRange>& lines() const noexcept {
        return m_lines;
    }

    [[nodiscard]] const std::string& note() const noexcept {
        return m_note;
    }

private:
    CodeLocation(std::string path, std::optional<LineRange> lines, std::string note)
        : m_path(std::move(path))
        , m_lines(lines)
        , m_note(std::move(note)) {}

    std::string m_path;
    std::optional<LineRange> m_lines;
    std::string m_note;
};

}
