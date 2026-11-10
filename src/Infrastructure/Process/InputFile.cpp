#include "Infrastructure/Process/InputFile.hpp"

#include <cstdint>
#include <format>
#include <fstream>
#include <random>
#include <system_error>
#include <utility>

namespace sloptour::infrastructure {

namespace {

constexpr std::string_view NAME_PREFIX = "sloptour-input-";
constexpr unsigned HALF_BITS = 32U;

[[nodiscard]] std::string unique_name() {
    std::random_device device;
    const auto high = static_cast<std::uint64_t>(device());
    const auto low = static_cast<std::uint64_t>(device());
    return std::format("{}{:016x}", NAME_PREFIX, (high << HALF_BITS) | low);
}

}

ProcessResult<InputFile> InputFile::create(std::string_view content) {
    std::error_code error;
    const auto directory = std::filesystem::temp_directory_path(error);
    if (error) {
        return std::unexpected(ProcessError("cannot find the temporary directory: " + error.message()));
    }
    InputFile file(directory / unique_name());
    std::ofstream stream(file.m_path, std::ios::binary | std::ios::trunc);
    stream.write(content.data(), static_cast<std::streamsize>(content.size()));
    stream.close();
    if (!stream) {
        return std::unexpected(ProcessError("cannot write the input to " + file.m_path_text));
    }
    return file;
}

InputFile::InputFile(std::filesystem::path path)
    : m_path(std::move(path))
    , m_path_text(m_path.string()) {}

InputFile::InputFile(InputFile&& other) noexcept
    : m_path(std::exchange(other.m_path, {}))
    , m_path_text(std::exchange(other.m_path_text, {})) {}

InputFile& InputFile::operator=(InputFile&& other) noexcept {
    if (this != &other) {
        remove();
        m_path = std::exchange(other.m_path, {});
        m_path_text = std::exchange(other.m_path_text, {});
    }
    return *this;
}

InputFile::~InputFile() {
    remove();
}

const std::string& InputFile::path() const noexcept {
    return m_path_text;
}

void InputFile::remove() noexcept {
    if (m_path.empty()) {
        return;
    }
    std::error_code error;
    std::filesystem::remove(m_path, error);
}

}
