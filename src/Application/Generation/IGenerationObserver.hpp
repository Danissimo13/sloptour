#pragma once

#include "Application/Generation/GenerationEvent.hpp"

namespace sloptour::application {

class IGenerationObserver {
public:
    IGenerationObserver() = default;
    virtual ~IGenerationObserver() = default;
    IGenerationObserver(const IGenerationObserver&) = delete;
    IGenerationObserver& operator=(const IGenerationObserver&) = delete;
    IGenerationObserver(IGenerationObserver&&) = delete;
    IGenerationObserver& operator=(IGenerationObserver&&) = delete;

    virtual void notify(const GenerationEvent& event) = 0;
};

}
