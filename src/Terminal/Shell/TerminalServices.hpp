#pragma once

#include "Application/Exploration/AskAboutStep.hpp"
#include "Application/Exploration/ComposeQuiz.hpp"
#include "Application/Exploration/DeepenStep.hpp"
#include "Application/Generation/GenerateTour.hpp"
#include "Application/Library/TourLibrary.hpp"
#include "Application/Library/Workspace.hpp"
#include "Application/Ports/IEditor.hpp"
#include "Application/Preferences/UserPreferences.hpp"

#include <memory>

namespace sloptour::terminal {

struct TerminalServices {
    std::shared_ptr<const application::TourLibrary> library;
    std::shared_ptr<const application::Workspace> workspace;
    std::shared_ptr<const application::GenerateTour> generator;
    std::shared_ptr<const application::AskAboutStep> asker;
    std::shared_ptr<const application::DeepenStep> deepener;
    std::shared_ptr<const application::ComposeQuiz> quizzer;
    std::shared_ptr<const application::IEditor> editor;
    std::shared_ptr<application::UserPreferences> preferences;
};

}
