#pragma once

#include "Application/Generation/IGenerationObserver.hpp"

#include <algorithm>
#include <mutex>
#include <vector>

namespace sloptour::tests {

class RecordingObserver final : public application::IGenerationObserver {
public:
    void notify(const application::GenerationEvent& event) override {
        const std::scoped_lock lock(m_mutex);
        m_events.push_back(event);
    }

    template <typename TEvent>
    [[nodiscard]] std::size_t count() const {
        const std::scoped_lock lock(m_mutex);
        return static_cast<std::size_t>(std::ranges::count_if(
            m_events, [](const application::GenerationEvent& event) { return std::holds_alternative<TEvent>(event); }));
    }

private:
    mutable std::mutex m_mutex;
    std::vector<application::GenerationEvent> m_events;
};

}
