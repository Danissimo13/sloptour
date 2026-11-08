#pragma once

#include "Core/Errors/Error.hpp"

#include <expected>

namespace sloptour::infrastructure {

struct ProcessContext {};

using ProcessError = core::Error<ProcessContext>;

template <typename T>
using ProcessResult = std::expected<T, ProcessError>;

}
