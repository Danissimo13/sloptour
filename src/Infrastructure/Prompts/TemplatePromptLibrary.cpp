#include "Infrastructure/Prompts/TemplatePromptLibrary.hpp"

#include "Infrastructure/Assets/AssetCatalog.hpp"
#include "Infrastructure/Assets/AssetPaths.hpp"
#include "Infrastructure/Prompts/PromptTemplate.hpp"
#include "Infrastructure/Storage/TextFiles.hpp"

namespace sloptour::infrastructure {

application::PromptResult<std::string>
TemplatePromptLibrary::render(std::string_view prompt, const application::PromptVariables& variables) const {
    auto text = template_of(prompt);
    if (!text) {
        return std::unexpected(text.error());
    }
    auto rendered = filled(*text, variables);
    if (!rendered) {
        return std::unexpected(rendered.error().located_at(prompt));
    }
    return rendered;
}

application::PromptResult<std::string> TemplatePromptLibrary::template_of(std::string_view prompt) const {
    const auto file_name = std::string(prompt) + std::string(asset_paths::PROMPT_EXTENSION);
    const auto custom = m_overrides / file_name;
    if (is_file(custom)) {
        auto text = read_text(custom);
        if (!text) {
            return std::unexpected(application::PromptError(text.error()));
        }
        return std::move(*text);
    }
    auto embedded = embedded_asset(std::string(asset_paths::PROMPTS_DIRECTORY) + file_name);
    if (!embedded) {
        return std::unexpected(application::PromptError(embedded.error()));
    }
    return std::string(*embedded);
}

}
