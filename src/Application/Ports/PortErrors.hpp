#pragma once

#include "Core/Errors/Error.hpp"

#include <expected>
#include <type_traits>
#include <utility>

namespace sloptour::application {

struct AgentContext {};
struct DecodeContext {};
struct PromptContext {};
struct StorageContext {};
struct InspectionContext {};
struct ApplicationContext {};

using AgentError = core::Error<AgentContext>;
using DecodeError = core::Error<DecodeContext>;
using PromptError = core::Error<PromptContext>;
using StorageError = core::Error<StorageContext>;
using InspectionError = core::Error<InspectionContext>;
using ApplicationError = core::Error<ApplicationContext>;

template <typename T>
using AgentResult = std::expected<T, AgentError>;
template <typename T>
using DecodeResult = std::expected<T, DecodeError>;
template <typename T>
using PromptResult = std::expected<T, PromptError>;
template <typename T>
using StorageResult = std::expected<T, StorageError>;
template <typename T>
using InspectionResult = std::expected<T, InspectionError>;
template <typename T>
using ApplicationResult = std::expected<T, ApplicationError>;

template <typename T, typename TContext>
[[nodiscard]] ApplicationResult<T> lifted(std::expected<T, core::Error<TContext>> result) {
    if (!result) {
        return std::unexpected(ApplicationError(result.error()));
    }
    if constexpr (std::is_void_v<T>) {
        return {};
    } else {
        return std::move(*result);
    }
}

}
