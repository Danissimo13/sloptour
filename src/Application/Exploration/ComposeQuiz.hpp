#pragma once

#include "Application/Generation/GenerationSettings.hpp"
#include "Application/Ports/AgentChoice.hpp"
#include "Application/Ports/AgentListener.hpp"
#include "Application/Ports/IAgentProvider.hpp"
#include "Application/Ports/IPromptLibrary.hpp"
#include "Application/Ports/IReplyDecoder.hpp"
#include "Application/Ports/PortErrors.hpp"
#include "Application/Preferences/ReplyLanguage.hpp"
#include "Domain/Quiz/Quiz.hpp"
#include "Domain/Tour/Tour.hpp"

#include <memory>
#include <stop_token>
#include <utility>

namespace sloptour::application {

class ComposeQuiz {
public:
    struct Dependencies {
        std::shared_ptr<const IAgentProvider> agents;
        std::shared_ptr<const IPromptLibrary> prompts;
        std::shared_ptr<const IReplyDecoder> decoder;
        GenerationSettings settings;
    };

    explicit ComposeQuiz(Dependencies dependencies)
        : m_dependencies(std::move(dependencies)) {}

    struct Request {
        domain::Tour tour;
        AgentChoice agent;
        ReplyLanguage language;
    };

    [[nodiscard]] ApplicationResult<domain::Quiz> execute(const Request& request, const AgentListener& listener,
                                                          const std::stop_token& stop) const;

private:
    Dependencies m_dependencies;
};

}
