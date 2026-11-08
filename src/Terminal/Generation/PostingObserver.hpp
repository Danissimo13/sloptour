#pragma once

#include "Application/Generation/IGenerationObserver.hpp"

#include <functional>
#include <utility>

namespace sloptour::terminal {

class PostingObserver final : public application::IGenerationObserver {
public:
    explicit PostingObserver(std::function<void(application::GenerationEvent)> post)
        : m_post(std::move(post)) {}

    void notify(const application::GenerationEvent& event) override {
        m_post(event);
    }

private:
    std::function<void(application::GenerationEvent)> m_post;
};

}
