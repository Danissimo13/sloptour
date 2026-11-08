#pragma once

#include "Application/Preferences/ReplyLanguage.hpp"
#include "Terminal/Localization/LanguagePack.hpp"

#include <cstddef>
#include <initializer_list>
#include <string>
#include <string_view>
#include <vector>

namespace sloptour::terminal {

class Localizer {
public:
    explicit Localizer(std::vector<LanguagePack> packs);

    [[nodiscard]] std::string text(std::string_view key) const;
    [[nodiscard]] std::string format(std::string_view key, std::initializer_list<std::string_view> values) const;
    [[nodiscard]] std::string count(std::size_t amount, std::string_view noun) const;
    [[nodiscard]] bool use(std::string_view code);
    [[nodiscard]] const LanguagePack& current() const noexcept;
    [[nodiscard]] const std::vector<LanguagePack>& packs() const noexcept;
    [[nodiscard]] application::ReplyLanguage reply_language() const;

private:
    [[nodiscard]] std::string_view plural_form(std::size_t amount) const;

    std::vector<LanguagePack> m_packs;
    std::size_t m_current = 0;
};

}
