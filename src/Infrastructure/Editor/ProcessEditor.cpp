#include "Infrastructure/Editor/ProcessEditor.hpp"

#include "Infrastructure/Platform/Platform.hpp"
#include "Infrastructure/Process/AttachedProcess.hpp"

#include <format>
#include <sstream>

namespace sloptour::infrastructure {

namespace {

constexpr std::string_view VISUAL = "VISUAL";
constexpr std::string_view EDITOR = "EDITOR";
constexpr std::string_view PATH_PLACEHOLDER = "{path}";
constexpr std::string_view LINE_PLACEHOLDER = "{line}";

[[nodiscard]] std::vector<std::string> words_of(const std::string& text) {
    std::vector<std::string> words;
    std::istringstream stream(text);
    for (std::string word; stream >> word;) {
        words.push_back(word);
    }
    return words;
}

[[nodiscard]] std::string replaced(std::string text, std::string_view placeholder, const std::string& value) {
    for (auto found = text.find(placeholder); found != std::string::npos;
         found = text.find(placeholder, found + value.size())) {
        text.replace(found, placeholder.size(), value);
    }
    return text;
}

}

application::StorageResult<void> ProcessEditor::open(std::string_view path, std::size_t line) const {
    const auto arguments = command(path, line);
    auto status = run_attached(arguments, m_root);
    if (!status) {
        return std::unexpected(application::StorageError(status.error()));
    }
    if (*status != 0) {
        return std::unexpected(
            application::StorageError(std::format("{} exited with status {}", arguments.front(), *status)));
    }
    return {};
}

std::vector<std::string> ProcessEditor::command(std::string_view path, std::size_t line) const {
    const auto configured = platform::environment_variable(VISUAL)
                                .or_else([] { return platform::environment_variable(EDITOR); })
                                .or_else([] { return platform::default_editor(); });
    auto words = words_of(configured.value_or(m_settings.fallback));
    if (words.empty()) {
        words.push_back(m_settings.fallback);
    }
    const auto name = std::filesystem::path(words.front()).filename().string();
    const auto pattern = m_settings.arguments.find(name);
    const auto& template_arguments =
        pattern == m_settings.arguments.end() ? m_settings.default_arguments : pattern->second;
    const auto file = (m_root / path).string();
    const auto number = std::to_string(line);
    for (const auto& argument : template_arguments) {
        words.push_back(replaced(replaced(argument, PATH_PLACEHOLDER, file), LINE_PLACEHOLDER, number));
    }
    return words;
}

}
