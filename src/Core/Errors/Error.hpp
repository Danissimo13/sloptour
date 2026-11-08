#pragma once

#include <string>
#include <string_view>
#include <utility>

namespace sloptour::core {

template <typename TContext>
class Error {
public:
    explicit Error(std::string message)
        : m_message(std::move(message)) {}

    template <typename TCauseContext>
    explicit Error(const Error<TCauseContext>& cause)
        : m_message(cause.message()) {}

    [[nodiscard]] const std::string& message() const noexcept {
        return m_message;
    }

    [[nodiscard]] Error located_at(std::string_view location) const {
        if (location.empty()) {
            return *this;
        }
        std::string located(location);
        located += LOCATION_SEPARATOR;
        located += m_message;
        return Error(std::move(located));
    }

private:
    static constexpr std::string_view LOCATION_SEPARATOR = ": ";

    std::string m_message;
};

template <typename TContext, typename TCauseContext>
[[nodiscard]] Error<TContext> rethrown_as(const Error<TCauseContext>& cause) {
    return Error<TContext>(cause);
}

}
