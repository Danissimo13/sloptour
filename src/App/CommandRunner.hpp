#pragma once

#include "App/Composition.hpp"
#include "App/Invocation.hpp"
#include "Application/Generation/GenerationRequest.hpp"
#include "Terminal/Shell/Destination.hpp"

#include <utility>

namespace sloptour::app {

class CommandRunner {
public:
    explicit CommandRunner(Composition composition)
        : m_composition(std::move(composition)) {}

    [[nodiscard]] int operator()(const BrowseCommand& command) const;
    [[nodiscard]] int operator()(const CreateCommand& command) const;
    [[nodiscard]] int operator()(const PlayCommand& command) const;
    [[nodiscard]] int operator()(const ListCommand& command) const;
    [[nodiscard]] int operator()(const AgentsCommand& command) const;
    [[nodiscard]] int operator()(const EjectPromptsCommand& command) const;

private:
    [[nodiscard]] int interactive(terminal::Destination destination) const;
    [[nodiscard]] int generate_plainly(const application::GenerationRequest& request) const;
    [[nodiscard]] application::GenerationRequest request_of(const CreateCommand& command) const;
    [[nodiscard]] application::ReplyLanguage reply_language() const;

    Composition m_composition;
};

}
