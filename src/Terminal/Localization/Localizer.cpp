#include "Terminal/Localization/Localizer.hpp"

#include "Core/Errors/Panic.hpp"

#include <format>
#include <functional>
#include <optional>
#include <utility>

namespace sloptour::terminal {

namespace {

constexpr std::string_view ONE = "one";
constexpr std::string_view FEW = "few";
constexpr std::string_view MANY = "many";
constexpr std::string_view PLURAL_PREFIX = "plural.";
constexpr std::size_t DECIMAL = 10;
constexpr std::size_t HUNDRED = 100;
constexpr std::size_t TEEN_FIRST = 11;
constexpr std::size_t TEEN_LAST = 14;
constexpr std::size_t FEW_FIRST = 2;
constexpr std::size_t FEW_LAST = 4;

using PluralRule = std::function<std::string_view(std::size_t)>;

[[nodiscard]] std::string_view one_or_many(std::size_t amount) {
    return amount == 1 ? ONE : MANY;
}

[[nodiscard]] std::string_view east_slavic(std::size_t amount) {
    const auto last = amount % DECIMAL;
    const auto teen = amount % HUNDRED >= TEEN_FIRST && amount % HUNDRED <= TEEN_LAST;
    if (last == 1 && !teen) {
        return ONE;
    }
    if (last >= FEW_FIRST && last <= FEW_LAST && !teen) {
        return FEW;
    }
    return MANY;
}

[[nodiscard]] PluralRule rule_named(std::string_view name) {
    const std::vector<std::pair<std::string_view, PluralRule>> rules{
        {"one-many", one_or_many},
        {"east-slavic", east_slavic},
    };
    for (const auto& [candidate, rule] : rules) {
        if (candidate == name) {
            return rule;
        }
    }
    return one_or_many;
}

[[nodiscard]] std::string replaced(std::string text, std::initializer_list<std::string_view> values) {
    std::size_t number = 1;
    for (const auto value : values) {
        const auto placeholder = std::format("{{{}}}", number++);
        for (auto found = text.find(placeholder); found != std::string::npos;
             found = text.find(placeholder, found + value.size())) {
            text.replace(found, placeholder.size(), value);
        }
    }
    return text;
}

}

Localizer::Localizer(std::vector<LanguagePack> packs)
    : m_packs(std::move(packs)) {
    if (m_packs.empty()) {
        core::panic("no interface language is available");
    }
}

std::string Localizer::text(std::string_view key) const {
    const auto lookup = [key](const LanguagePack& pack) -> std::optional<std::string> {
        const auto found = pack.texts.find(key);
        return found == pack.texts.end() ? std::nullopt : std::optional(found->second);
    };
    return lookup(m_packs.at(m_current)).or_else([&] { return lookup(m_packs.front()); }).value_or(std::string(key));
}

std::string Localizer::format(std::string_view key, std::initializer_list<std::string_view> values) const {
    return replaced(text(key), values);
}

std::string Localizer::count(std::size_t amount, std::string_view noun) const {
    const auto key = std::string(PLURAL_PREFIX) + std::string(noun) + "." + std::string(plural_form(amount));
    return format(key, {std::to_string(amount)});
}

bool Localizer::use(std::string_view code) {
    for (std::size_t index = 0; index < m_packs.size(); ++index) {
        if (m_packs.at(index).code == code) {
            m_current = index;
            return true;
        }
    }
    return false;
}

const LanguagePack& Localizer::current() const noexcept {
    return m_packs.at(m_current);
}

const std::vector<LanguagePack>& Localizer::packs() const noexcept {
    return m_packs;
}

application::ReplyLanguage Localizer::reply_language() const {
    return {.code = current().code, .name = current().agent_name};
}

std::string_view Localizer::plural_form(std::size_t amount) const {
    return rule_named(current().plural_rule)(amount);
}

}
