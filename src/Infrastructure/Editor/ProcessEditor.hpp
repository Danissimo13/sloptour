#pragma once

#include "Application/Ports/IEditor.hpp"
#include "Infrastructure/Editor/EditorSettings.hpp"

#include <filesystem>
#include <string>
#include <utility>
#include <vector>

namespace sloptour::infrastructure {

class ProcessEditor final : public application::IEditor {
public:
    ProcessEditor(std::filesystem::path root, EditorSettings settings)
        : m_root(std::move(root))
        , m_settings(std::move(settings)) {}

    [[nodiscard]] application::StorageResult<void> open(std::string_view path, std::size_t line) const override;

private:
    [[nodiscard]] std::vector<std::string> command(std::string_view path, std::size_t line) const;

    std::filesystem::path m_root;
    EditorSettings m_settings;
};

}
