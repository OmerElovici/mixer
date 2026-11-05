#pragma once


#include <cstdint>
#include <span>

class ISamplesProcessor {
public:
    [[nodiscard]] virtual bool process_samples(std::span<const std::int16_t> samples) = 0;
    [[nodiscard]] virtual bool start() = 0;
    [[nodiscard]] virtual bool is_ready() = 0;

    ISamplesProcessor(const ISamplesProcessor&) = default;
    ISamplesProcessor(ISamplesProcessor&&) = default;
    ISamplesProcessor& operator=(const ISamplesProcessor&) = default;
    ISamplesProcessor& operator=(ISamplesProcessor&&) = default;
    virtual ~ISamplesProcessor() = default;

    ISamplesProcessor() = default;
};
