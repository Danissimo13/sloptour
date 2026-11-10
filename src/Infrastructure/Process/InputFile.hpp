#pragma once

#include "Infrastructure/Process/ProcessError.hpp"

#include <filesystem>
#include <string>
#include <string_view>

namespace sloptour::infrastructure {

class InputFile {
public:
    [[nodiscard]] static ProcessResult<InputFile> create(std::string_view content);

    InputFile(const InputFile&) = delete;
    InputFile& operator=(const InputFile&) = delete;
    InputFile(InputFile&& other) noexcept;
    InputFile& operator=(InputFile&& other) noexcept;
    ~InputFile();

    [[nodiscard]] const std::string& path() const noexcept;

private:
    explicit InputFile(std::filesystem::path path);

    void remove() noexcept;

    std::filesystem::path m_path;
    std::string m_path_text;
};

}
