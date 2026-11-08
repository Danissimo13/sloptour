#include "Application/Preferences/LanguageInstruction.hpp"

#include "Core/Text/Text.hpp"

namespace sloptour::application {

namespace {

constexpr std::string_view FALLBACK_LANGUAGE = "English";

}

std::string language_of(const ReplyLanguage& language) {
    return core::is_blank(language.name) ? std::string(FALLBACK_LANGUAGE) : language.name;
}

}
