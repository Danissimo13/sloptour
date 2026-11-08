#pragma once

#include "Application/Preferences/ReplyLanguage.hpp"

#include <string>

namespace sloptour::application {

[[nodiscard]] std::string language_of(const ReplyLanguage& language);

}
