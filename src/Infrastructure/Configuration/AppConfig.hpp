#pragma once

#include "Application/Generation/GenerationSettings.hpp"
#include "Application/Preferences/Preferences.hpp"
#include "Infrastructure/Agents/AgentProfile.hpp"
#include "Infrastructure/Editor/EditorSettings.hpp"
#include "Infrastructure/Git/GitSettings.hpp"
#include "Infrastructure/Json/JsonAccess.hpp"

#include <string>
#include <vector>

namespace sloptour::infrastructure {

struct AppConfig {
    std::string default_agent;
    std::vector<AgentProfile> agents;
    application::GenerationSettings generation;
    GitSettings git;
    std::string storage_directory;
    EditorSettings editor;
    application::Preferences preferences;
    Json terminal;
};

}
