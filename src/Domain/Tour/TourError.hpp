#pragma once

#include "Core/Errors/Error.hpp"

#include <expected>

namespace sloptour::domain {

struct TourContext {};

using TourError = core::Error<TourContext>;

template <typename T>
using TourResult = std::expected<T, TourError>;

}
