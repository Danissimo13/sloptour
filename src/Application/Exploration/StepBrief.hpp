#pragma once

#include "Domain/Progress/Exchange.hpp"
#include "Domain/Tour/Step.hpp"
#include "Domain/Tour/Tour.hpp"

#include <span>
#include <string>

namespace sloptour::application {

[[nodiscard]] std::string brief_of(const domain::Step& step);
[[nodiscard]] std::string history_of(std::span<const domain::Exchange> exchanges);
[[nodiscard]] std::string digest_of(const domain::Tour& tour);

}
