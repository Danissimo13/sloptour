#pragma once

#include "Infrastructure/Json/JsonAccess.hpp"

#include <string>
#include <string_view>
#include <utility>

namespace sloptour::infrastructure {

class JsonObjectBuilder {
public:
    template <typename TValue>
    JsonObjectBuilder& with(std::string_view key, TValue&& value) {
        m_object.emplace(std::string(key), Json(std::forward<TValue>(value)));
        return *this;
    }

    [[nodiscard]] Json build() {
        return std::move(m_object);
    }

private:
    Json m_object = Json::object();
};

}
