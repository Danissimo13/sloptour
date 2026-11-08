#pragma once

#include "Application/Exploration/ComposeQuiz.hpp"
#include "Application/Generation/AgentDialogue.hpp"
#include "Application/Generation/GenerationContext.hpp"
#include "Application/Generation/GenerationRequest.hpp"
#include "Application/Generation/GenerationSettings.hpp"
#include "Application/Generation/IGenerationObserver.hpp"
#include "Application/Generation/StepWriter.hpp"
#include "Application/Ports/IAgentProvider.hpp"
#include "Application/Ports/IClock.hpp"
#include "Application/Ports/IPromptLibrary.hpp"
#include "Application/Ports/IReplyDecoder.hpp"
#include "Application/Ports/IRepositoryInspector.hpp"
#include "Application/Ports/ITourRepository.hpp"
#include "Application/Ports/PortErrors.hpp"
#include "Domain/Outline/TourOutline.hpp"
#include "Domain/Tour/Step.hpp"
#include "Domain/Tour/Tour.hpp"
#include "Domain/Tour/TourId.hpp"

#include <cstddef>
#include <memory>
#include <stop_token>
#include <vector>

namespace sloptour::application {

class GenerateTour {
public:
    struct Dependencies {
        std::shared_ptr<const IAgentProvider> agents;
        std::shared_ptr<const IPromptLibrary> prompts;
        std::shared_ptr<const IReplyDecoder> decoder;
        std::shared_ptr<ITourRepository> tours;
        std::shared_ptr<const IRepositoryInspector> inspector;
        std::shared_ptr<const IClock> clock;
        std::shared_ptr<const ComposeQuiz> quiz;
        GenerationSettings settings;
    };

    explicit GenerateTour(Dependencies dependencies);

    [[nodiscard]] ApplicationResult<domain::TourId>
    execute(const GenerationRequest& request, IGenerationObserver& observer, const std::stop_token& stop) const;

private:
    using ChapterSteps = ApplicationResult<std::vector<domain::Step>>;

    [[nodiscard]] ApplicationResult<GenerationContext> prepare(const GenerationRequest& request,
                                                               IGenerationObserver& observer) const;
    [[nodiscard]] ApplicationResult<domain::TourOutline> draft_outline(const AgentDialogue& dialogue,
                                                                       const GenerationContext& context,
                                                                       IGenerationObserver& observer,
                                                                       const std::stop_token& stop) const;
    [[nodiscard]] std::vector<ChapterSteps> write_chapters(const StepWriter& writer, const GenerationContext& context,
                                                           const domain::TourOutline& outline,
                                                           IGenerationObserver& observer,
                                                           const std::stop_token& stop) const;
    [[nodiscard]] ApplicationResult<domain::Tour>
    build(const GenerationRequest& request, const GenerationContext& context, const domain::TourOutline& outline,
          const std::shared_ptr<const AgentDialogue>& dialogue, IGenerationObserver& observer,
          const std::stop_token& stop) const;
    void add_quiz(domain::Tour& tour, const GenerationRequest& request, const GenerationContext& context,
                  IGenerationObserver& observer, const std::stop_token& stop) const;
    [[nodiscard]] ApplicationResult<domain::Tour>
    assemble(const GenerationRequest& request, const GenerationContext& context, const domain::TourOutline& outline,
             const std::vector<ChapterSteps>& chapters, std::string_view agent) const;
    [[nodiscard]] ApplicationResult<domain::TourId> identify(const GenerationRequest& request,
                                                             const GenerationContext& context) const;

    Dependencies m_dependencies;
};

}
